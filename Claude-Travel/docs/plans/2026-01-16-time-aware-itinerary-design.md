# Time-Aware Itinerary System Design

## Overview

Transform the trip planning system from loose time slots (morning/afternoon/evening) to a coherent, time-aware itinerary where activities have actual timestamps, durations are considered, and travel time between locations is accounted for.

## Architecture: Two-Pass System

**Pass 1: Research Phase** (existing agents, enhanced)
- Agents find good options without worrying about scheduling
- Each item includes duration and timing metadata

**Pass 2: Scheduling Phase** (new)
- Scheduler agent receives ALL items from Pass 1
- Builds day-by-day timeline with actual start times
- Creates option groups when conflicts arise
- Outputs fully scheduled itinerary

---

## Implementation Tasks

### Task 1: Update Activities Agent
**Files:** `backend/config/harness/agents/activities.yaml`

**Changes:**
1. Add to output schema:
   ```yaml
   duration_minutes: number (required, realistic estimate)
   operating_hours: "string (e.g., '09:00-18:00')"
   typical_start_times: ["string (e.g., '09:00', '14:00')"]
   ```

2. Update prompt to emphasize:
   - Duration estimates must be realistic (include time inside, not just "2 hours" for a massive museum)
   - Operating hours are critical for scheduling
   - Note if activity is time-specific (e.g., sunset viewpoint, timed entry)

3. Add validation:
   - `duration_minutes` required on all items
   - Duration must be between 30-480 minutes (sanity check)

**Dependencies:** None
**Parallel:** Yes - can run with Tasks 2, 3

---

### Task 2: Update Dining Agent
**Files:** `backend/config/harness/agents/dining.yaml`

**Changes:**
1. Add to output schema:
   ```yaml
   meal_type: "breakfast|lunch|dinner|snack"
   typical_duration_minutes: number (30-120 typical)
   typical_time_range: "string (e.g., '12:00-14:00' for lunch)"
   ```

2. Update prompt:
   - Specify which meal each restaurant is for
   - Include realistic dining duration (quick lunch vs leisurely dinner)
   - Note reservation times if booking required

3. Add validation:
   - `meal_type` required
   - `typical_duration_minutes` required

**Dependencies:** None
**Parallel:** Yes - can run with Tasks 1, 3

---

### Task 3: Update Transport Agent
**Files:** `backend/config/harness/agents/transport.yaml`

**Changes:**
1. Add to output schema:
   ```yaml
   duration_minutes: number (travel time)
   from_location: { lat, lng, name }
   to_location: { lat, lng, name }
   typical_frequency: "string|null (e.g., 'every 30 min')"
   ```

2. Update prompt:
   - Focus on inter-area travel times (hotel area to attractions, between neighborhoods)
   - Include buffer time for waiting/walking to station
   - Note if transport is time-dependent (last train, etc.)

3. Add validation:
   - `duration_minutes` required
   - `from_location` and `to_location` required

**Dependencies:** None
**Parallel:** Yes - can run with Tasks 1, 2

---

### Task 4: Update Flights Agent Output Mapping
**Files:** `backend/src/services/orchestrator.ts`

**Changes:**
1. In `mapFlightsToItems`, ensure these fields are preserved in metadata:
   ```typescript
   arrival_time: string (ISO datetime)
   departure_time: string (ISO datetime)
   arrival_airport: string
   departure_airport: string
   flight_type: 'outbound' | 'return'
   ```

2. Create helper function to extract flight constraints:
   ```typescript
   extractFlightConstraints(flightResults: AgentResult): {
     outbound_arrival: { date: string, time: string, airport: string }
     return_departure: { date: string, time: string, airport: string }
   }
   ```

**Dependencies:** None
**Parallel:** Yes - can run with Tasks 1-3

---

### Task 5: Create Scheduler Agent
**Files:** `backend/config/harness/agents/scheduler.yaml` (new file)

**Prompt structure:**

```yaml
agent_name: scheduler
description: "Builds coherent day-by-day timeline from research results"

prompt:
  system: |
    You are a TRIP SCHEDULER. Your job is to take researched activities,
    dining, and transport options and build a coherent daily timeline.

    ═══════════════════════════════════════════════════════════
    INPUTS PROVIDED
    ═══════════════════════════════════════════════════════════
    - Flight times (when travelers arrive/depart)
    - Lodging info (check-in/check-out times, locations)
    - Activities (with durations and operating hours)
    - Restaurants (with meal types and durations)
    - Transport estimates (travel times between areas)

    ═══════════════════════════════════════════════════════════
    YOUR TASK: DAY-BY-DAY SCHEDULING
    ═══════════════════════════════════════════════════════════
    For EACH day, you must:

    1. IDENTIFY CONSTRAINTS
       - Day 1: Travel day? When does flight arrive?
       - Last day: When must they leave for airport?
       - Inter-city travel days: Account for transport time

    2. CALCULATE AVAILABLE TIME
       - Start time: Flight arrival, or 09:00 for full days
       - End time: Reasonable evening (21:00-22:00), or airport departure buffer
       - Subtract meal times (~2.5 hours total for 3 meals)

    3. ASSIGN START TIMES
       - Place activities with actual start times
       - Add travel time between locations (estimate 20-30 min if not specified)
       - Ensure operating hours are respected
       - Place meals at appropriate times (breakfast 08-09, lunch 12-14, dinner 19-21)

    4. DETECT CONFLICTS
       - If activities don't fit: Create option groups
       - Option groups should offer meaningfully different choices
       - Label groups clearly: "morning-activity", "afternoon-choice"

    5. OUTPUT EACH ITEM WITH:
       - day: number
       - start_time: "HH:MM" (24-hour format)
       - end_time: "HH:MM"
       - travel_to_next_minutes: number
       - option_group: string|null (e.g., "day3-afternoon" if part of a choice)

    ═══════════════════════════════════════════════════════════
    OPTION GROUP RULES
    ═══════════════════════════════════════════════════════════
    - Only create when genuinely needed (time conflict)
    - Each option in a group must be a complete alternative
    - Maximum 2-3 options per group
    - All options in a group have same option_group value
    - Include option_label: "A", "B", "C" to distinguish

    ═══════════════════════════════════════════════════════════
    OUTPUT FORMAT
    ═══════════════════════════════════════════════════════════
    {
      "scheduled_items": [
        {
          "id": "string (from original item)",
          "name": "string",
          "type": "activity|restaurant|transport",
          "day": number,
          "start_time": "HH:MM",
          "end_time": "HH:MM",
          "duration_minutes": number,
          "travel_to_next_minutes": number,
          "option_group": "string|null",
          "option_label": "string|null (A, B, C)",
          "location": { lat, lng, address }
        }
      ],
      "day_summaries": [
        {
          "day": 1,
          "date": "2026-06-11",
          "theme": "Arrival & Porto Introduction",
          "available_hours": 6,
          "scheduled_hours": 5.5,
          "has_options": false
        }
      ],
      "option_groups": [
        {
          "id": "day3-afternoon",
          "day": 3,
          "time_slot": "14:00-18:00",
          "reason": "Both activities are 3+ hours, only time for one",
          "options": ["Option A: LX Factory", "Option B: Tile Museum"]
        }
      ],
      "coverage": {
        "total_days": 9,
        "days_with_activities": 9,
        "total_items_scheduled": 45,
        "option_groups_created": 2,
        "meals_scheduled": 27
      }
    }

    ═══════════════════════════════════════════════════════════
    SELF-CHECK BEFORE RESPONDING
    ═══════════════════════════════════════════════════════════
    □ Every day has scheduled items?
    □ Arrival day respects flight landing time?
    □ Departure day has airport buffer (2+ hours before flight)?
    □ No activity starts before previous one ends?
    □ Operating hours respected for all activities?
    □ Meals placed at reasonable times?
    □ Travel time included between different locations?
    □ Math is correct: start_time + duration = end_time?

  variables:
    - trip_context (dates, destinations, travelers)
    - flight_constraints (arrival/departure times)
    - lodging_info (locations, check-in/out)
    - activities (from activities agent)
    - restaurants (from dining agent)
    - transport_estimates (from transport agent)

validation:
  max_retries: 2
  checks:
    - type: array_not_empty
      arrayPath: scheduled_items
      error_message: "No items scheduled"

    - type: all_days_covered
      error_message: "Day {day} has no scheduled items"

    - type: no_time_overlaps
      error_message: "Time overlap on day {day}: {item1} and {item2}"

    - type: flight_constraints_respected
      error_message: "Activities scheduled outside available time on {day}"

    - type: time_math_valid
      error_message: "Time calculation error on {item}: {start} + {duration} != {end}"
```

**Dependencies:** Tasks 1-4 (needs updated agent outputs to schedule)
**Parallel:** No - must be created after understanding updated agent outputs

---

### Task 6: Update Orchestrator for Two-Pass System
**Files:** `backend/src/services/orchestrator.ts`, `backend/config/harness/orchestrator.yaml`

**Changes to orchestrator.yaml:**
```yaml
phases:
  - name: "research"
    parallel: true
    agents: ["flights", "lodging"]
    timeout: 120000

  - name: "details"
    parallel: true
    agents: ["dining", "activities", "transport"]
    timeout: 120000

  - name: "scheduling"  # NEW PHASE
    parallel: false
    agents: ["scheduler"]
    timeout: 90000
    inject_previous: true  # Flag to inject all previous results

  - name: "enrichment"
    parallel: false
    agents: ["visual"]
    timeout: 60000

  - name: "formatting"
    parallel: false
    agents: ["format"]
    timeout: 60000
```

**Changes to orchestrator.ts:**

1. Add method to build Scheduler context:
   ```typescript
   private buildSchedulerContext(
     tripContext: TripContext,
     previousResults: AgentResults
   ): SchedulerInput {
     return {
       trip_context: {
         start_date: tripContext.startDate,
         end_date: tripContext.endDate,
         destinations: tripContext.destinations,
         num_travelers: tripContext.numTravelers
       },
       flight_constraints: this.extractFlightConstraints(previousResults.flights),
       lodging_info: this.extractLodgingInfo(previousResults.lodging),
       activities: previousResults.activities?.output?.activities || [],
       restaurants: previousResults.dining?.output?.restaurants || [],
       transport_estimates: previousResults.transport?.output?.transport || []
     };
   }
   ```

2. Modify `buildAgentPrompt` to handle scheduler specially:
   ```typescript
   if (config.agentName === 'scheduler') {
     const schedulerContext = this.buildSchedulerContext(tripContext, previousResults);
     prompt = prompt.replace('{trip_context}', JSON.stringify(schedulerContext.trip_context));
     prompt = prompt.replace('{flight_constraints}', JSON.stringify(schedulerContext.flight_constraints));
     // ... etc for each variable
   }
   ```

3. Add Scheduler-specific validation:
   ```typescript
   private validateSchedulerOutput(output: SchedulerOutput, context: TripContext): ValidationResult {
     const errors: string[] = [];

     // Check all days covered
     for (let day = 1; day <= context.numDays; day++) {
       if (!output.scheduled_items.some(i => i.day === day)) {
         errors.push(`Day ${day} has no scheduled items`);
       }
     }

     // Check no overlaps
     // Check flight constraints
     // Check time math
     // ... (implement all validation checks)

     return { valid: errors.length === 0, errors };
   }
   ```

**Dependencies:** Task 5 (Scheduler agent must exist)
**Parallel:** No - sequential after Task 5

---

### Task 7: Update Database Schema for Time Fields
**Files:** `backend/src/services/database.ts`

**Changes:**
1. Add columns to itinerary_items (if not exists):
   ```sql
   ALTER TABLE itinerary_items ADD COLUMN start_time TEXT;
   ALTER TABLE itinerary_items ADD COLUMN end_time TEXT;
   ALTER TABLE itinerary_items ADD COLUMN duration_minutes INTEGER;
   ALTER TABLE itinerary_items ADD COLUMN travel_to_next_minutes INTEGER;
   ALTER TABLE itinerary_items ADD COLUMN option_label TEXT;
   ```

2. Update `CreateItineraryItemInput` type:
   ```typescript
   interface CreateItineraryItemInput {
     // ... existing fields
     start_time?: string;
     end_time?: string;
     duration_minutes?: number;
     travel_to_next_minutes?: number;
     option_label?: string;
   }
   ```

3. Update insert/update statements to handle new fields

**Dependencies:** None
**Parallel:** Yes - can run with Tasks 1-4

---

### Task 8: Add Option Selection Endpoint
**Files:** `backend/src/routes/trips.ts`

**New endpoint:**
```typescript
POST /api/trips/:id/select-option
Body: {
  option_group: string,    // e.g., "day3-afternoon"
  selected_label: string   // e.g., "A"
}

Response: {
  success: true,
  updated_items: [...],    // Items that were kept
  removed_items: [...],    // Items that were removed (other options)
  recalculated: boolean    // Whether downstream times were adjusted
}
```

**Logic:**
1. Find all items with matching `option_group`
2. Keep items where `option_label === selected_label`
3. Delete items where `option_label !== selected_label`
4. Optionally recalculate downstream times (or leave buffer as free time)

**Dependencies:** Task 7 (needs new DB fields)
**Parallel:** No - after Task 7

---

### Task 9: Update Frontend Types
**Files:** `frontend/src/types/trip.ts`

**Add to ItineraryItem:**
```typescript
interface ItineraryItem {
  // ... existing fields

  // Time-aware fields
  startTime?: string;      // "HH:MM" format
  endTime?: string;        // "HH:MM" format
  durationMinutes?: number;
  travelToNextMinutes?: number;

  // Option group fields
  optionGroup?: string;    // e.g., "day3-afternoon"
  optionLabel?: string;    // "A", "B", "C"
}
```

**Add new types:**
```typescript
interface OptionGroup {
  id: string;
  day: number;
  timeSlot: string;
  reason: string;
  options: OptionChoice[];
}

interface OptionChoice {
  label: string;
  itemIds: string[];
  summary: string;
}

interface DaySummary {
  day: number;
  date: string;
  theme: string;
  availableHours: number;
  scheduledHours: number;
  hasOptions: boolean;
}
```

**Dependencies:** None
**Parallel:** Yes - can run with backend tasks

---

### Task 10: Create Timeline Display Component
**Files:** `frontend/src/components/trip/DayTimeline.tsx` (new)

**Component structure:**
```tsx
interface DayTimelineProps {
  day: number;
  date: string;
  items: ItineraryItem[];
  optionGroups: OptionGroup[];
  onSelectOption: (groupId: string, label: string) => void;
}

// Renders:
// - Vertical timeline with time markers
// - Items positioned by start_time
// - Travel indicators between items (dotted line + duration)
// - Option group sections with selection UI
// - Visual distinction for meals vs activities vs transport
```

**Sub-components needed:**
- `TimelineItem` - Individual item card with time
- `TravelIndicator` - Shows travel time between items
- `OptionGroupCard` - Pick-one selection UI
- `TimeMarker` - Hour markers on the timeline

**Dependencies:** Task 9 (needs updated types)
**Parallel:** No - after Task 9

---

### Task 11: Create Option Selection Component
**Files:** `frontend/src/components/trip/OptionGroupSelector.tsx` (new)

**Component:**
```tsx
interface OptionGroupSelectorProps {
  group: OptionGroup;
  items: ItineraryItem[];
  selectedLabel?: string;
  onSelect: (label: string) => void;
  isLoading?: boolean;
}

// Renders:
// - Card showing "Choose one" with reason
// - Option A, B, C as selectable cards
// - Each shows: name, duration, key highlights
// - Selected state visual feedback
// - Calls API on selection
```

**Dependencies:** Task 9 (needs types), Task 8 (needs API)
**Parallel:** No - after Tasks 8, 9

---

### Task 12: Update DayView to Use Timeline
**Files:** `frontend/src/components/trip/DayView.tsx` (or equivalent)

**Changes:**
1. Replace loose morning/afternoon/evening grouping with `DayTimeline`
2. Sort items by `startTime` instead of `timeSlot`
3. Integrate option group selection
4. Show day summary (theme, hours scheduled)
5. Handle loading state during option selection

**Dependencies:** Tasks 10, 11 (needs new components)
**Parallel:** No - after Tasks 10, 11

---

### Task 13: Integration Testing
**Files:** `backend/src/tests/scheduler.test.ts` (new)

**Test cases:**
1. Basic scheduling - items get valid start times
2. Flight constraint - arrival day doesn't schedule before landing
3. Departure constraint - last day has airport buffer
4. Conflict detection - too many activities creates option groups
5. Time math - start + duration = end for all items
6. No overlaps - consecutive items don't overlap
7. Operating hours - activities within their open times
8. Full trip - 7-day trip schedules completely

**Dependencies:** Tasks 5, 6 (needs Scheduler working)
**Parallel:** No - after core implementation

---

## Execution Order

```
Phase A (Parallel):
├── Task 1: Update Activities Agent
├── Task 2: Update Dining Agent
├── Task 3: Update Transport Agent
├── Task 4: Update Flights Output Mapping
├── Task 7: Update Database Schema
└── Task 9: Update Frontend Types

Phase B (Sequential, after Phase A):
└── Task 5: Create Scheduler Agent

Phase C (Sequential, after Task 5):
└── Task 6: Update Orchestrator

Phase D (Sequential, after Tasks 6, 7):
└── Task 8: Add Option Selection Endpoint

Phase E (Sequential, after Tasks 8, 9):
├── Task 10: Create Timeline Component
└── Task 11: Create Option Selector Component

Phase F (Sequential, after Phase E):
└── Task 12: Update DayView

Phase G (After all):
└── Task 13: Integration Testing
```

---

## Success Criteria

1. **Activities have real times**: Every item shows "10:30 AM - 12:00 PM" not just "Morning"
2. **Travel is visible**: Users see "15 min walk" between activities
3. **Days are coherent**: No impossible schedules (museum at midnight, 10 activities in 4 hours)
4. **Flights respected**: Arrival day starts after landing, departure day ends with buffer
5. **Conflicts surfaced**: When too much is planned, users choose via option groups
6. **Validation catches errors**: Harness rejects bad schedules, retries with feedback

---

## Notes for Sub-Agents

- Each task should be completable in isolation once dependencies are met
- Tasks in the same phase can be assigned to parallel agents
- When modifying YAML configs, preserve existing structure and add new fields
- When modifying TypeScript, maintain existing type safety
- Test each piece incrementally - don't wait for full integration
- If a task is unclear, check the related files for patterns to follow

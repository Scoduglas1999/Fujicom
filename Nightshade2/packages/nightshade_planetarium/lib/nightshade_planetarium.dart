/// Nightshade Planetarium - Flutter GPU sky renderer
library nightshade_planetarium;

// Core models
export 'src/sky_view.dart';
export 'src/celestial_object.dart';
export 'src/coordinate_system.dart';

// Catalogs
export 'src/catalogs/catalog.dart';
export 'src/catalogs/star_catalog.dart';
export 'src/catalogs/constellation_data.dart';
export 'src/catalogs/catalog_manager.dart';
export 'src/catalogs/hyperleda_catalog.dart';
export 'src/catalogs/annotation_catalog.dart';

// Astronomy calculations
export 'src/astronomy/astronomy_calculations.dart';
export 'src/astronomy/planetary_positions.dart';
export 'src/astronomy/milky_way_data.dart';

// Rendering
export 'src/rendering/sky_renderer.dart';
export 'src/rendering/render_quality.dart';
export 'src/rendering/fov_overlays.dart';

// Catalogs (additional)
export 'src/catalogs/spatial_index.dart';

// Services
export 'src/services/survey_image_service.dart';
export 'src/services/mosaic_planner.dart';
export 'src/services/geolocation_service.dart';

// Planning
export 'src/planning/target_scoring.dart';

// Providers
export 'src/providers/planetarium_providers.dart';
export 'src/providers/catalog_providers.dart';
export 'src/providers/planning_providers.dart';
export 'src/providers/target_queue_provider.dart';
export 'src/providers/platform_providers.dart';
export 'src/providers/performance_providers.dart';

// Widgets
export 'src/widgets/interactive_sky_view.dart';
export 'src/widgets/framing_view.dart';
export 'src/widgets/time_control_panel.dart';
export 'src/widgets/object_details_panel.dart';
export 'src/widgets/compass_hud.dart';
export 'src/widgets/sky_minimap.dart';
export 'src/widgets/adaptive_layout.dart';

import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:intl/intl.dart';
import 'package:nightshade_ui/nightshade_ui.dart';
import 'package:nightshade_core/nightshade_core.dart';
import 'package:nightshade_core/src/database/database.dart' as db;

class ProfilesTab extends ConsumerWidget {
  const ProfilesTab({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    return Padding(
      padding: const EdgeInsets.all(24),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Header
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    'Equipment Profiles',
                    style: TextStyle(
                      fontSize: 18,
                      fontWeight: FontWeight.w600,
                      color: colors.textPrimary,
                    ),
                  ),
                  const SizedBox(height: 4),
                  Text(
                    'Save and load equipment configurations',
                    style: TextStyle(
                      fontSize: 12,
                      color: colors.textMuted,
                    ),
                  ),
                ],
              ),
              _CreateProfileButton(colors: colors),
            ],
          ),

          const SizedBox(height: 24),

          // Profiles grid
          Expanded(
            child: ref.watch(allProfilesProvider).when(
              data: (profiles) {
                if (profiles.isEmpty) {
                  return _ProfilesEmptyState(colors: colors);
                }
                return GridView.builder(
                  gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
                    crossAxisCount: 3,
                    mainAxisSpacing: 16,
                    crossAxisSpacing: 16,
                    childAspectRatio: 1.4,
                  ),
                  itemCount: profiles.length + 1, // include quick-create tile
                  itemBuilder: (context, index) {
                    if (index == profiles.length) {
                      return _EmptyProfileCard(colors: colors);
                    }
                    final profile = profiles[index];
                    return _ProfileCard(
                      profile: profile,
                      colors: colors,
                    );
                  },
                );
              },
              loading: () => Center(
                child: CircularProgressIndicator(color: colors.primary),
              ),
              error: (error, stack) => _ProfilesError(
                error: error,
                colors: colors,
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class _CreateProfileButton extends ConsumerStatefulWidget {
  final NightshadeColors colors;

  const _CreateProfileButton({required this.colors});

  @override
  ConsumerState<_CreateProfileButton> createState() => _CreateProfileButtonState();
}

class _CreateProfileButtonState extends ConsumerState<_CreateProfileButton> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    return MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      child: GestureDetector(
        onTap: () async {
          try {
            final profileService = ref.read(profileServiceProvider);
            await profileService.createProfile('New Profile');
            if (mounted) {
              ScaffoldMessenger.of(context).showSnackBar(
                const SnackBar(content: Text('Profile created')),
              );
            }
          } catch (e) {
            if (mounted) {
              ScaffoldMessenger.of(context).showSnackBar(
                SnackBar(content: Text('Failed to create profile: $e')),
              );
            }
          }
        },
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 150),
          padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 10),
          decoration: BoxDecoration(
            gradient: LinearGradient(
              colors: [
                widget.colors.primary,
                widget.colors.primary.withValues(alpha: 0.8),
              ],
            ),
            borderRadius: BorderRadius.circular(10),
            boxShadow: _isHovered
                ? [
                    BoxShadow(
                      color: widget.colors.primary.withValues(alpha: 0.4),
                      blurRadius: 12,
                      offset: const Offset(0, 4),
                    ),
                  ]
                : null,
          ),
          child: const Row(
            children: [
              Icon(LucideIcons.plus, size: 16, color: Colors.white),
              SizedBox(width: 8),
              Text(
                'Create Profile',
                style: TextStyle(
                  fontSize: 13,
                  fontWeight: FontWeight.w600,
                  color: Colors.white,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _ProfilesEmptyState extends StatelessWidget {
  final NightshadeColors colors;

  const _ProfilesEmptyState({required this.colors});

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(LucideIcons.scan, size: 48, color: colors.textMuted),
          const SizedBox(height: 12),
          Text(
            'No equipment profiles yet',
            style: TextStyle(
              fontSize: 16,
              fontWeight: FontWeight.w600,
              color: colors.textPrimary,
            ),
          ),
          const SizedBox(height: 8),
          Text(
            'Create a profile to store your hardware assignments and defaults.',
            textAlign: TextAlign.center,
            style: TextStyle(
              fontSize: 12,
              color: colors.textSecondary,
            ),
          ),
          const SizedBox(height: 20),
          _CreateProfileButton(colors: colors),
        ],
      ),
    );
  }
}

class _ProfilesError extends StatelessWidget {
  final Object error;
  final NightshadeColors colors;

  const _ProfilesError({required this.error, required this.colors});

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(LucideIcons.alertTriangle, size: 48, color: colors.error),
          const SizedBox(height: 12),
          Text(
            'Failed to load profiles',
            style: TextStyle(
              fontSize: 16,
              fontWeight: FontWeight.w600,
              color: colors.textPrimary,
            ),
          ),
          const SizedBox(height: 6),
          Text(
            error.toString(),
            textAlign: TextAlign.center,
            style: TextStyle(
              fontSize: 12,
              color: colors.textSecondary,
            ),
          ),
        ],
      ),
    );
  }
}

class _ProfileCard extends ConsumerStatefulWidget {
  final db.EquipmentProfile profile;
  final NightshadeColors colors;

  const _ProfileCard({
    required this.profile,
    required this.colors,
  });

  @override
  ConsumerState<_ProfileCard> createState() => _ProfileCardState();
}

class _ProfileCardState extends ConsumerState<_ProfileCard> {
  bool _isHovered = false;
  bool _isWorking = false;

  @override
  Widget build(BuildContext context) {
    final profile = widget.profile;
    final lastUsed = DateFormat.yMMMd().format(profile.updatedAt);
    final equipment = _buildEquipmentList(profile);

    return MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      child: AnimatedContainer(
        duration: const Duration(milliseconds: 200),
        padding: const EdgeInsets.all(20),
        decoration: BoxDecoration(
          color: widget.colors.surface,
          borderRadius: BorderRadius.circular(16),
          border: Border.all(
            color: profile.isActive
                ? widget.colors.primary
                : _isHovered
                    ? widget.colors.primary.withValues(alpha: 0.5)
                    : widget.colors.border,
            width: profile.isActive ? 2 : 1,
          ),
          boxShadow: _isHovered
              ? [
                  BoxShadow(
                    color: widget.colors.primary.withValues(alpha: 0.1),
                    blurRadius: 20,
                    offset: const Offset(0, 8),
                  ),
                ]
              : null,
        ),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Header
            Row(
              children: [
                Container(
                  width: 40,
                  height: 40,
                  decoration: BoxDecoration(
                    gradient: profile.isActive
                        ? LinearGradient(
                            colors: [
                              widget.colors.primary.withValues(alpha: 0.2),
                              widget.colors.primary.withValues(alpha: 0.1),
                            ],
                          )
                        : null,
                    color: profile.isActive ? null : widget.colors.surfaceAlt,
                    borderRadius: BorderRadius.circular(10),
                    border: profile.isActive
                        ? Border.all(
                            color: widget.colors.primary.withValues(alpha: 0.3),
                          )
                        : null,
                  ),
                  child: Icon(
                    LucideIcons.scan,
                    size: 18,
                    color: profile.isActive
                        ? widget.colors.primary
                        : widget.colors.textSecondary,
                  ),
                ),
                const SizedBox(width: 12),
                Expanded(
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Text(
                        profile.name,
                        style: TextStyle(
                          fontSize: 14,
                          fontWeight: FontWeight.w600,
                          color: widget.colors.textPrimary,
                        ),
                      ),
                      const SizedBox(height: 2),
                      Text(
                        'Last updated: $lastUsed',
                        style: TextStyle(
                          fontSize: 10,
                          color: widget.colors.textMuted,
                        ),
                      ),
                    ],
                  ),
                ),
                if (profile.isActive)
                  Container(
                    padding: const EdgeInsets.symmetric(
                      horizontal: 8,
                      vertical: 4,
                    ),
                    decoration: BoxDecoration(
                      color: widget.colors.success.withValues(alpha: 0.15),
                      borderRadius: BorderRadius.circular(12),
                      border: Border.all(
                        color: widget.colors.success.withValues(alpha: 0.3),
                      ),
                    ),
                    child: Text(
                      'Active',
                      style: TextStyle(
                        fontSize: 9,
                        fontWeight: FontWeight.w600,
                        color: widget.colors.success,
                      ),
                    ),
                  ),
              ],
            ),

            const SizedBox(height: 16),

            // Equipment list
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: equipment.map((item) {
                  return Padding(
                    padding: const EdgeInsets.only(bottom: 4),
                    child: Row(
                      children: [
                        Container(
                          width: 4,
                          height: 4,
                          decoration: BoxDecoration(
                            shape: BoxShape.circle,
                            color: widget.colors.textMuted,
                          ),
                        ),
                        const SizedBox(width: 8),
                        Expanded(
                          child: Text(
                            item,
                            style: TextStyle(
                              fontSize: 11,
                              color: widget.colors.textSecondary,
                            ),
                            overflow: TextOverflow.ellipsis,
                          ),
                        ),
                      ],
                    ),
                  );
                }).toList(),
              ),
            ),

            // Actions
            Row(
              children: [
                Expanded(
                  child: _ProfileAction(
                    icon: profile.isActive ? LucideIcons.check : LucideIcons.play,
                    label: profile.isActive
                        ? 'Active'
                        : _isWorking
                            ? 'Loading...'
                            : 'Load',
                    isPrimary: !profile.isActive,
                    isBusy: _isWorking,
                    colors: widget.colors,
                    onTap: profile.isActive || _isWorking ? null : _loadProfile,
                  ),
                ),
                const SizedBox(width: 8),
                _IconAction(
                  icon: LucideIcons.copy,
                  colors: widget.colors,
                  onTap: _isWorking ? null : _duplicateProfile,
                ),
                const SizedBox(width: 4),
                _IconAction(
                  icon: LucideIcons.trash2,
                  colors: widget.colors,
                  onTap: _isWorking ? null : _confirmDeleteProfile,
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  List<String> _buildEquipmentList(db.EquipmentProfile profile) {
    final items = <String>[];
    if (profile.cameraId != null) {
      items.add('Camera: ${_formatDeviceId(profile.cameraId!)}');
    }
    if (profile.mountId != null) {
      items.add('Mount: ${_formatDeviceId(profile.mountId!)}');
    }
    if (profile.focuserId != null) {
      items.add('Focuser: ${_formatDeviceId(profile.focuserId!)}');
    }
    if (profile.filterWheelId != null) {
      items.add('Filter Wheel: ${_formatDeviceId(profile.filterWheelId!)}');
    }
    if (items.isEmpty) {
      items.add('No equipment assigned');
    }
    final optics = _formatOptics(profile);
    if (optics != null) {
      items.add(optics);
    }
    return items.take(4).toList();
  }

  String? _formatOptics(db.EquipmentProfile profile) {
    if (profile.focalLength <= 0 && profile.aperture <= 0) {
      return null;
    }
    final focal = profile.focalLength > 0 ? '${profile.focalLength.toStringAsFixed(0)}mm' : '--';
    final aperture = profile.aperture > 0 ? '${profile.aperture.toStringAsFixed(0)}mm' : '--';
    final ratio = profile.focalRatio != null
        ? 'f/${profile.focalRatio!.toStringAsFixed(1)}'
        : '--';
    return 'Optics: $focal · $aperture · $ratio';
  }

  String _formatDeviceId(String id) {
    if (id.contains('.')) {
      return id.split('.').last;
    }
    return id;
  }

  Future<void> _loadProfile() async {
    setState(() => _isWorking = true);
    final profile = widget.profile;
    try {
      final autoConnectSetting = ref.read(appSettingsProvider).valueOrNull?.autoConnectEquipment ?? false;
      await ref.read(profileServiceProvider).loadProfile(profile.id, autoConnect: autoConnectSetting);
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Loaded "${profile.name}"'),
            backgroundColor: widget.colors.success,
          ),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to load profile: $e'),
            backgroundColor: widget.colors.error,
          ),
        );
      }
    } finally {
      if (mounted) {
        setState(() => _isWorking = false);
      }
    }
  }

  Future<void> _duplicateProfile() async {
    final profile = widget.profile;
    setState(() => _isWorking = true);
    try {
      final newName = '${profile.name} Copy';
      await ref.read(profileServiceProvider).duplicateProfile(profile.id, newName);
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Duplicated "${profile.name}"'),
            backgroundColor: widget.colors.success,
          ),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to duplicate profile: $e'),
            backgroundColor: widget.colors.error,
          ),
        );
      }
    } finally {
      if (mounted) {
        setState(() => _isWorking = false);
      }
    }
  }

  Future<void> _confirmDeleteProfile() async {
    final profile = widget.profile;
    final colors = widget.colors;
    final shouldDelete = await showDialog<bool>(
      context: context,
      builder: (context) {
        return AlertDialog(
          backgroundColor: colors.surface,
          title: Text(
            'Delete Profile',
            style: TextStyle(color: colors.textPrimary),
          ),
          content: Text(
            'Delete "${profile.name}"? This cannot be undone.',
            style: TextStyle(color: colors.textSecondary),
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context, false),
              child: Text('Cancel', style: TextStyle(color: colors.textMuted)),
            ),
            TextButton(
              onPressed: () => Navigator.pop(context, true),
              child: Text('Delete', style: TextStyle(color: colors.error)),
            ),
          ],
        );
      },
    );
    if (shouldDelete == true) {
      await _deleteProfile();
    }
  }

  Future<void> _deleteProfile() async {
    final profile = widget.profile;
    setState(() => _isWorking = true);
    try {
      await ref.read(profileServiceProvider).deleteProfile(profile.id);
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Deleted "${profile.name}"'),
            backgroundColor: widget.colors.success,
          ),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to delete profile: $e'),
            backgroundColor: widget.colors.error,
          ),
        );
      }
    } finally {
      if (mounted) {
        setState(() => _isWorking = false);
      }
    }
  }
}

class _ProfileAction extends ConsumerStatefulWidget {
  final IconData icon;
  final String label;
  final bool isPrimary;
  final NightshadeColors colors;
  final VoidCallback? onTap;
  final bool isBusy;

  const _ProfileAction({
    required this.icon,
    required this.label,
    this.isPrimary = false,
    required this.colors,
    this.onTap,
    this.isBusy = false,
  });

  @override
  ConsumerState<_ProfileAction> createState() => _ProfileActionState();
}

class _ProfileActionState extends ConsumerState<_ProfileAction> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    final canInteract = widget.onTap != null && !widget.isBusy;
    return MouseRegion(
      onEnter: (_) {
        if (canInteract) setState(() => _isHovered = true);
      },
      onExit: (_) {
        if (_isHovered) setState(() => _isHovered = false);
      },
      child: GestureDetector(
        onTap: canInteract ? widget.onTap : null,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 150),
          padding: const EdgeInsets.symmetric(vertical: 8),
          decoration: BoxDecoration(
            color: widget.isPrimary
                ? widget.colors.primary
                : _isHovered
                    ? widget.colors.surfaceAlt
                    : widget.colors.background,
            borderRadius: BorderRadius.circular(8),
            border: widget.isPrimary
                ? null
                : Border.all(color: widget.colors.border),
          ),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              if (widget.isBusy) ...[
                SizedBox(
                  width: 12,
                  height: 12,
                  child: CircularProgressIndicator(
                    strokeWidth: 2,
                    valueColor: AlwaysStoppedAnimation<Color>(
                      widget.isPrimary ? Colors.white : widget.colors.textSecondary,
                    ),
                  ),
                ),
                const SizedBox(width: 6),
              ] else ...[
                Icon(
                  widget.icon,
                  size: 12,
                  color: widget.isPrimary
                      ? Colors.white
                      : widget.colors.textSecondary,
                ),
                const SizedBox(width: 6),
              ],
              Text(
                widget.label,
                style: TextStyle(
                  fontSize: 11,
                  fontWeight: FontWeight.w500,
                  color: widget.isPrimary
                      ? Colors.white
                      : widget.colors.textSecondary,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _IconAction extends ConsumerStatefulWidget {
  final IconData icon;
  final NightshadeColors colors;
  final VoidCallback? onTap;

  const _IconAction({
    required this.icon,
    required this.colors,
    this.onTap,
  });

  @override
  ConsumerState<_IconAction> createState() => _IconActionState();
}

class _IconActionState extends ConsumerState<_IconAction> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    final isDisabled = widget.onTap == null;
    return MouseRegion(
      onEnter: (_) {
        if (!isDisabled) setState(() => _isHovered = true);
      },
      onExit: (_) {
        if (_isHovered) setState(() => _isHovered = false);
      },
      child: GestureDetector(
        onTap: isDisabled ? null : widget.onTap,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 150),
          width: 32,
          height: 32,
          decoration: BoxDecoration(
            color: _isHovered
                ? widget.colors.surfaceAlt
                : widget.colors.background,
            borderRadius: BorderRadius.circular(8),
            border: Border.all(color: widget.colors.border),
          ),
          child: Icon(
            widget.icon,
            size: 14,
            color: isDisabled
                ? widget.colors.textMuted.withValues(alpha: 0.4)
                : widget.colors.textMuted,
          ),
        ),
      ),
    );
  }
}

class _EmptyProfileCard extends ConsumerStatefulWidget {
  final NightshadeColors colors;

  const _EmptyProfileCard({required this.colors});

  @override
  ConsumerState<_EmptyProfileCard> createState() => _EmptyProfileCardState();
}

class _EmptyProfileCardState extends ConsumerState<_EmptyProfileCard> {
  bool _isHovered = false;
  bool _isCreating = false;

  @override
  Widget build(BuildContext context) {
    return MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      child: GestureDetector(
        onTap: _isCreating ? null : _createProfile,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 200),
          decoration: BoxDecoration(
            color: _isHovered
                ? widget.colors.surfaceAlt
                : widget.colors.background,
            borderRadius: BorderRadius.circular(16),
            border: Border.all(
              color: _isHovered
                  ? widget.colors.primary.withValues(alpha: 0.5)
                  : widget.colors.border,
              style: BorderStyle.solid,
            ),
          ),
          child: Center(
            child: Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                Container(
                  width: 48,
                  height: 48,
                  decoration: BoxDecoration(
                    color: widget.colors.surfaceAlt,
                    shape: BoxShape.circle,
                    border: Border.all(
                      color: widget.colors.border,
                      style: BorderStyle.solid,
                    ),
                  ),
                  child: _isCreating
                      ? Padding(
                          padding: const EdgeInsets.all(12),
                          child: CircularProgressIndicator(
                            strokeWidth: 2,
                            valueColor: AlwaysStoppedAnimation<Color>(
                              widget.colors.primary,
                            ),
                          ),
                        )
                      : Icon(
                          LucideIcons.plus,
                          size: 24,
                          color: widget.colors.textMuted,
                        ),
                ),
                const SizedBox(height: 12),
                Text(
                  'Create New Profile',
                  style: TextStyle(
                    fontSize: 13,
                    fontWeight: FontWeight.w500,
                    color: widget.colors.textSecondary,
                  ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }

  Future<void> _createProfile() async {
    setState(() => _isCreating = true);
    try {
      final profileService = ref.read(profileServiceProvider);
      await profileService.createProfile('New Profile');
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: const Text('Profile created'),
            backgroundColor: widget.colors.success,
          ),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to create profile: $e'),
            backgroundColor: widget.colors.error,
          ),
        );
      }
    } finally {
      if (mounted) {
        setState(() => _isCreating = false);
      }
    }
  }
}

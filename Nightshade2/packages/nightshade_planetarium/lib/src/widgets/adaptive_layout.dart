import 'package:flutter/material.dart';

/// Form factors for responsive layout
enum FormFactor { phone, tablet, desktop }

/// Determines the form factor based on screen dimensions
FormFactor getFormFactor(BuildContext context) {
  final shortestSide = MediaQuery.of(context).size.shortestSide;
  if (shortestSide < 600) return FormFactor.phone;
  if (shortestSide < 900) return FormFactor.tablet;
  return FormFactor.desktop;
}

/// Provides adaptive sizing values based on form factor
class AdaptiveSizing {
  final FormFactor formFactor;

  AdaptiveSizing(this.formFactor);

  /// Creates adaptive sizing from BuildContext
  factory AdaptiveSizing.of(BuildContext context) {
    return AdaptiveSizing(getFormFactor(context));
  }

  /// Size for compass HUD widget
  double get compassSize => switch (formFactor) {
        FormFactor.phone => 60,
        FormFactor.tablet => 90,
        FormFactor.desktop => 80,
      };

  /// Size for sky minimap widget
  double get minimapSize => switch (formFactor) {
        FormFactor.phone => 80,
        FormFactor.tablet => 120,
        FormFactor.desktop => 100,
      };

  /// Edge padding for HUD elements
  double get edgePadding => switch (formFactor) {
        FormFactor.phone => 12,
        _ => 16,
      };

  /// Whether to use bottom sheet instead of side panel for details
  bool get useBottomSheet => formFactor == FormFactor.phone;

  /// Whether to use condensed HUD (hide secondary info)
  bool get useCondensedHud => formFactor == FormFactor.phone;

  /// Font scale for HUD text elements
  double get hudTextScale => switch (formFactor) {
        FormFactor.phone => 0.85,
        FormFactor.tablet => 1.0,
        FormFactor.desktop => 1.0,
      };

  /// Touch target minimum size
  double get minTouchTarget => switch (formFactor) {
        FormFactor.phone => 44,
        FormFactor.tablet => 44,
        FormFactor.desktop => 32,
      };
}

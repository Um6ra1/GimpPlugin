/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

/* NOTE: This file is autogenerated by enumcode.pl */

#ifndef __GIMP_ENUMS_H__
#define __GIMP_ENUMS_H__

G_BEGIN_DECLS


#define GIMP_TYPE_BRUSH_APPLICATION_MODE (gimp_brush_application_mode_get_type ())

GType gimp_brush_application_mode_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_BRUSH_HARD,
  GIMP_BRUSH_SOFT
} GimpBrushApplicationMode;


#define GIMP_TYPE_BRUSH_GENERATED_SHAPE (gimp_brush_generated_shape_get_type ())

GType gimp_brush_generated_shape_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_BRUSH_GENERATED_CIRCLE,
  GIMP_BRUSH_GENERATED_SQUARE,
  GIMP_BRUSH_GENERATED_DIAMOND
} GimpBrushGeneratedShape;


#define GIMP_TYPE_CONVERT_DITHER_TYPE (gimp_convert_dither_type_get_type ())

GType gimp_convert_dither_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_NO_DITHER,
  GIMP_FS_DITHER,
  GIMP_FSLOWBLEED_DITHER,
  GIMP_FIXED_DITHER
} GimpConvertDitherType;


#define GIMP_TYPE_CONVERT_PALETTE_TYPE (gimp_convert_palette_type_get_type ())

GType gimp_convert_palette_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_MAKE_PALETTE,
  GIMP_REUSE_PALETTE,
  GIMP_WEB_PALETTE,
  GIMP_MONO_PALETTE,
  GIMP_CUSTOM_PALETTE
} GimpConvertPaletteType;


#define GIMP_TYPE_CONVOLUTION_TYPE (gimp_convolution_type_get_type ())

GType gimp_convolution_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_NORMAL_CONVOL,
  GIMP_ABSOLUTE_CONVOL,
  GIMP_NEGATIVE_CONVOL
} GimpConvolutionType;


#define GIMP_TYPE_CONVOLVE_TYPE (gimp_convolve_type_get_type ())

GType gimp_convolve_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_BLUR_CONVOLVE,
  GIMP_SHARPEN_CONVOLVE
} GimpConvolveType;


#define GIMP_TYPE_FILL_TYPE (gimp_fill_type_get_type ())

GType gimp_fill_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_FOREGROUND_FILL,
  GIMP_BACKGROUND_FILL,
  GIMP_WHITE_FILL,
  GIMP_TRANSPARENT_FILL,
  GIMP_PATTERN_FILL
} GimpFillType;


#define GIMP_TYPE_GRADIENT_SEGMENT_COLOR (gimp_gradient_segment_color_get_type ())

GType gimp_gradient_segment_color_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_GRADIENT_SEGMENT_RGB,
  GIMP_GRADIENT_SEGMENT_HSV_CCW,
  GIMP_GRADIENT_SEGMENT_HSV_CW
} GimpGradientSegmentColor;


#define GIMP_TYPE_GRADIENT_SEGMENT_TYPE (gimp_gradient_segment_type_get_type ())

GType gimp_gradient_segment_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_GRADIENT_SEGMENT_LINEAR,
  GIMP_GRADIENT_SEGMENT_CURVED,
  GIMP_GRADIENT_SEGMENT_SINE,
  GIMP_GRADIENT_SEGMENT_SPHERE_INCREASING,
  GIMP_GRADIENT_SEGMENT_SPHERE_DECREASING
} GimpGradientSegmentType;


#define GIMP_TYPE_HISTOGRAM_CHANNEL (gimp_histogram_channel_get_type ())

GType gimp_histogram_channel_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_HISTOGRAM_VALUE,
  GIMP_HISTOGRAM_RED,
  GIMP_HISTOGRAM_GREEN,
  GIMP_HISTOGRAM_BLUE,
  GIMP_HISTOGRAM_ALPHA
} GimpHistogramChannel;


#define GIMP_TYPE_HUE_RANGE (gimp_hue_range_get_type ())

GType gimp_hue_range_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_ALL_HUES,
  GIMP_RED_HUES,
  GIMP_YELLOW_HUES,
  GIMP_GREEN_HUES,
  GIMP_CYAN_HUES,
  GIMP_BLUE_HUES,
  GIMP_MAGENTA_HUES
} GimpHueRange;


#define GIMP_TYPE_INK_BLOB_TYPE (gimp_ink_blob_type_get_type ())

GType gimp_ink_blob_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_INK_BLOB_TYPE_CIRCLE,
  GIMP_INK_BLOB_TYPE_SQUARE,
  GIMP_INK_BLOB_TYPE_DIAMOND
} GimpInkBlobType;


#define GIMP_TYPE_LAYER_MODE_EFFECTS (gimp_layer_mode_effects_get_type ())

GType gimp_layer_mode_effects_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_NORMAL_MODE,
  GIMP_DISSOLVE_MODE,
  GIMP_BEHIND_MODE,
  GIMP_MULTIPLY_MODE,
  GIMP_SCREEN_MODE,
  GIMP_OVERLAY_MODE,
  GIMP_DIFFERENCE_MODE,
  GIMP_ADDITION_MODE,
  GIMP_SUBTRACT_MODE,
  GIMP_DARKEN_ONLY_MODE,
  GIMP_LIGHTEN_ONLY_MODE,
  GIMP_HUE_MODE,
  GIMP_SATURATION_MODE,
  GIMP_COLOR_MODE,
  GIMP_VALUE_MODE,
  GIMP_DIVIDE_MODE,
  GIMP_DODGE_MODE,
  GIMP_BURN_MODE,
  GIMP_HARDLIGHT_MODE,
  GIMP_SOFTLIGHT_MODE,
  GIMP_GRAIN_EXTRACT_MODE,
  GIMP_GRAIN_MERGE_MODE,
  GIMP_COLOR_ERASE_MODE
} GimpLayerModeEffects;


#define GIMP_TYPE_MASK_APPLY_MODE (gimp_mask_apply_mode_get_type ())

GType gimp_mask_apply_mode_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_MASK_APPLY,
  GIMP_MASK_DISCARD
} GimpMaskApplyMode;


#define GIMP_TYPE_MERGE_TYPE (gimp_merge_type_get_type ())

GType gimp_merge_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_EXPAND_AS_NECESSARY,
  GIMP_CLIP_TO_IMAGE,
  GIMP_CLIP_TO_BOTTOM_LAYER,
  GIMP_FLATTEN_IMAGE
} GimpMergeType;


#define GIMP_TYPE_OFFSET_TYPE (gimp_offset_type_get_type ())

GType gimp_offset_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_OFFSET_BACKGROUND,
  GIMP_OFFSET_TRANSPARENT
} GimpOffsetType;


#define GIMP_TYPE_ORIENTATION_TYPE (gimp_orientation_type_get_type ())

GType gimp_orientation_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_ORIENTATION_HORIZONTAL,
  GIMP_ORIENTATION_VERTICAL,
  GIMP_ORIENTATION_UNKNOWN
} GimpOrientationType;


#define GIMP_TYPE_ROTATION_TYPE (gimp_rotation_type_get_type ())

GType gimp_rotation_type_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_ROTATE_90,
  GIMP_ROTATE_180,
  GIMP_ROTATE_270
} GimpRotationType;


#define GIMP_TYPE_SELECT_CRITERION (gimp_select_criterion_get_type ())

GType gimp_select_criterion_get_type (void) G_GNUC_CONST;

typedef enum
{
  GIMP_SELECT_CRITERION_COMPOSITE,
  GIMP_SELECT_CRITERION_R,
  GIMP_SELECT_CRITERION_G,
  GIMP_SELECT_CRITERION_B,
  GIMP_SELECT_CRITERION_H,
  GIMP_SELECT_CRITERION_S,
  GIMP_SELECT_CRITERION_V
} GimpSelectCriterion;


void           gimp_enums_init           (void);

const gchar ** gimp_enums_get_type_names (gint *n_type_names);


G_END_DECLS

#endif /* __GIMP_ENUMS_H__ */
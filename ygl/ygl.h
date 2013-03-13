#ifndef _YGL_YGL_
#define _YGL_YGL_

#include "ygltypes.h"

namespace ygl{

enum VERSION_1_1_enum{
	/* VERSION_1_1 enum: */
	GL_VERSION_1_1					= 1,
	/* passthru: AttribMask */
	GL_DEPTH_BUFFER_BIT				= 0x00000100,/*# AttribMask*/
	GL_STENCIL_BUFFER_BIT				= 0x00000400,/*# AttribMask*/
	GL_COLOR_BUFFER_BIT				= 0x00004000,/*# AttribMask*/
	/* passthru: Boolean */
	GL_FALSE						= 0	,/*# Boolean*/
	GL_TRUE						= 1	,/*# Boolean*/
	/* passthru: BeginMode */
	GL_POINTS						= 0x0000,/*# BeginMode*/
	GL_LINES						= 0x0001,/*# BeginMode*/
	GL_LINE_LOOP					= 0x0002,/*# BeginMode*/
	GL_LINE_STRIP					= 0x0003,/*# BeginMode*/
	GL_TRIANGLES					= 0x0004,/*# BeginMode*/
	GL_TRIANGLE_STRIP					= 0x0005,/*# BeginMode*/
	GL_TRIANGLE_FAN					= 0x0006,/*# BeginMode*/
	GL_QUADS						= 0x0007,/*# BeginMode*/
	/* passthru: AlphaFunction */
	GL_NEVER						= 0x0200,/*# AlphaFunction*/
	GL_LESS						= 0x0201,/*# AlphaFunction*/
	GL_EQUAL						= 0x0202,/*# AlphaFunction*/
	GL_LEQUAL						= 0x0203,/*# AlphaFunction*/
	GL_GREATER						= 0x0204,/*# AlphaFunction*/
	GL_NOTEQUAL					= 0x0205,/*# AlphaFunction*/
	GL_GEQUAL						= 0x0206,/*# AlphaFunction*/
	GL_ALWAYS						= 0x0207,/*# AlphaFunction*/
	/* passthru: BlendingFactorDest */
	GL_ZERO						= 0	,/*# BlendingFactorDest*/
	GL_ONE						= 1	,/*# BlendingFactorDest*/
	GL_SRC_COLOR					= 0x0300,/*# BlendingFactorDest*/
	GL_ONE_MINUS_SRC_COLOR				= 0x0301,/*# BlendingFactorDest*/
	GL_SRC_ALPHA					= 0x0302,/*# BlendingFactorDest*/
	GL_ONE_MINUS_SRC_ALPHA				= 0x0303,/*# BlendingFactorDest*/
	GL_DST_ALPHA					= 0x0304,/*# BlendingFactorDest*/
	GL_ONE_MINUS_DST_ALPHA				= 0x0305,/*# BlendingFactorDest*/
	/* passthru: BlendingFactorSrc */
	GL_DST_COLOR					= 0x0306,/*# BlendingFactorSrc*/
	GL_ONE_MINUS_DST_COLOR				= 0x0307,/*# BlendingFactorSrc*/
	GL_SRC_ALPHA_SATURATE				= 0x0308,/*# BlendingFactorSrc*/
	/* passthru: DrawBufferMode */
	GL_NONE						= 0	,/*# DrawBufferMode*/
	GL_FRONT_LEFT					= 0x0400,/*# DrawBufferMode*/
	GL_FRONT_RIGHT					= 0x0401,/*# DrawBufferMode*/
	GL_BACK_LEFT					= 0x0402,/*# DrawBufferMode*/
	GL_BACK_RIGHT					= 0x0403,/*# DrawBufferMode*/
	GL_FRONT						= 0x0404,/*# DrawBufferMode*/
	GL_BACK						= 0x0405,/*# DrawBufferMode*/
	GL_LEFT						= 0x0406,/*# DrawBufferMode*/
	GL_RIGHT						= 0x0407,/*# DrawBufferMode*/
	GL_FRONT_AND_BACK					= 0x0408,/*# DrawBufferMode*/
	/* passthru: ErrorCode */
	GL_NO_ERROR					= 0	,/*# ErrorCode*/
	GL_INVALID_ENUM					= 0x0500,/*# ErrorCode*/
	GL_INVALID_VALUE					= 0x0501,/*# ErrorCode*/
	GL_INVALID_OPERATION				= 0x0502,/*# ErrorCode*/
	GL_OUT_OF_MEMORY					= 0x0505,/*# ErrorCode*/
	/* passthru: FrontFaceDirection */
	GL_CW						= 0x0900,/*# FrontFaceDirection*/
	GL_CCW						= 0x0901,/*# FrontFaceDirection*/
	/* passthru: GetPName */
	GL_POINT_SIZE					= 0x0B11,/*# 1 F	# GetPName*/
	GL_POINT_SIZE_RANGE				= 0x0B12,/*# 2 F	# GetPName*/
	GL_POINT_SIZE_GRANULARITY				= 0x0B13,/*# 1 F	# GetPName*/
	GL_LINE_SMOOTH					= 0x0B20,/*# 1 I	# GetPName*/
	GL_LINE_WIDTH					= 0x0B21,/*# 1 F	# GetPName*/
	GL_LINE_WIDTH_RANGE				= 0x0B22,/*# 2 F	# GetPName*/
	GL_LINE_WIDTH_GRANULARITY				= 0x0B23,/*# 1 F	# GetPName*/
	GL_POLYGON_MODE					= 0x0B40,/*# 2 I	# GetPName*/
	GL_POLYGON_SMOOTH					= 0x0B41,/*# 1 I	# GetPName*/
	GL_CULL_FACE					= 0x0B44,/*# 1 I	# GetPName*/
	GL_CULL_FACE_MODE					= 0x0B45,/*# 1 I	# GetPName*/
	GL_FRONT_FACE					= 0x0B46,/*# 1 I	# GetPName*/
	GL_DEPTH_RANGE					= 0x0B70,/*# 2 F	# GetPName*/
	GL_DEPTH_TEST					= 0x0B71,/*# 1 I	# GetPName*/
	GL_DEPTH_WRITEMASK					= 0x0B72,/*# 1 I	# GetPName*/
	GL_DEPTH_CLEAR_VALUE				= 0x0B73,/*# 1 F	# GetPName*/
	GL_DEPTH_FUNC					= 0x0B74,/*# 1 I	# GetPName*/
	GL_STENCIL_TEST					= 0x0B90,/*# 1 I	# GetPName*/
	GL_STENCIL_CLEAR_VALUE				= 0x0B91,/*# 1 I	# GetPName*/
	GL_STENCIL_FUNC					= 0x0B92,/*# 1 I	# GetPName*/
	GL_STENCIL_VALUE_MASK				= 0x0B93,/*# 1 I	# GetPName*/
	GL_STENCIL_FAIL					= 0x0B94,/*# 1 I	# GetPName*/
	GL_STENCIL_PASS_DEPTH_FAIL				= 0x0B95,/*# 1 I	# GetPName*/
	GL_STENCIL_PASS_DEPTH_PASS				= 0x0B96,/*# 1 I	# GetPName*/
	GL_STENCIL_REF					= 0x0B97,/*# 1 I	# GetPName*/
	GL_STENCIL_WRITEMASK				= 0x0B98,/*# 1 I	# GetPName*/
	GL_VIEWPORT					= 0x0BA2,/*# 4 I	# GetPName*/
	GL_DITHER						= 0x0BD0,/*# 1 I	# GetPName*/
	GL_BLEND_DST					= 0x0BE0,/*# 1 I	# GetPName*/
	GL_BLEND_SRC					= 0x0BE1,/*# 1 I	# GetPName*/
	GL_BLEND						= 0x0BE2,/*# 1 I	# GetPName*/
	GL_LOGIC_OP_MODE					= 0x0BF0,/*# 1 I	# GetPName*/
	GL_COLOR_LOGIC_OP					= 0x0BF2,/*# 1 I	# GetPName*/
	GL_DRAW_BUFFER					= 0x0C01,/*# 1 I	# GetPName*/
	GL_READ_BUFFER					= 0x0C02,/*# 1 I	# GetPName*/
	GL_SCISSOR_BOX					= 0x0C10,/*# 4 I	# GetPName*/
	GL_SCISSOR_TEST					= 0x0C11,/*# 1 I	# GetPName*/
	GL_COLOR_CLEAR_VALUE				= 0x0C22,/*# 4 F	# GetPName*/
	GL_COLOR_WRITEMASK					= 0x0C23,/*# 4 I	# GetPName*/
	GL_DOUBLEBUFFER					= 0x0C32,/*# 1 I	# GetPName*/
	GL_STEREO						= 0x0C33,/*# 1 I	# GetPName*/
	GL_LINE_SMOOTH_HINT				= 0x0C52,/*# 1 I	# GetPName*/
	GL_POLYGON_SMOOTH_HINT				= 0x0C53,/*# 1 I	# GetPName*/
	GL_UNPACK_SWAP_BYTES				= 0x0CF0,/*# 1 I	# GetPName*/
	GL_UNPACK_LSB_FIRST				= 0x0CF1,/*# 1 I	# GetPName*/
	GL_UNPACK_ROW_LENGTH				= 0x0CF2,/*# 1 I	# GetPName*/
	GL_UNPACK_SKIP_ROWS				= 0x0CF3,/*# 1 I	# GetPName*/
	GL_UNPACK_SKIP_PIXELS				= 0x0CF4,/*# 1 I	# GetPName*/
	GL_UNPACK_ALIGNMENT				= 0x0CF5,/*# 1 I	# GetPName*/
	GL_PACK_SWAP_BYTES					= 0x0D00,/*# 1 I	# GetPName*/
	GL_PACK_LSB_FIRST					= 0x0D01,/*# 1 I	# GetPName*/
	GL_PACK_ROW_LENGTH					= 0x0D02,/*# 1 I	# GetPName*/
	GL_PACK_SKIP_ROWS					= 0x0D03,/*# 1 I	# GetPName*/
	GL_PACK_SKIP_PIXELS				= 0x0D04,/*# 1 I	# GetPName*/
	GL_PACK_ALIGNMENT					= 0x0D05,/*# 1 I	# GetPName*/
	GL_MAX_TEXTURE_SIZE				= 0x0D33,/*# 1 I	# GetPName*/
	GL_MAX_VIEWPORT_DIMS				= 0x0D3A,/*# 2 F	# GetPName*/
	GL_SUBPIXEL_BITS					= 0x0D50,/*# 1 I	# GetPName*/
	GL_TEXTURE_1D					= 0x0DE0,/*# 1 I	# GetPName*/
	GL_TEXTURE_2D					= 0x0DE1,/*# 1 I	# GetPName*/
	GL_POLYGON_OFFSET_UNITS				= 0x2A00,/*# 1 F	# GetPName*/
	GL_POLYGON_OFFSET_POINT				= 0x2A01,/*# 1 I	# GetPName*/
	GL_POLYGON_OFFSET_LINE				= 0x2A02,/*# 1 I	# GetPName*/
	GL_POLYGON_OFFSET_FILL				= 0x8037,/*# 1 I	# GetPName*/
	GL_POLYGON_OFFSET_FACTOR				= 0x8038,/*# 1 F	# GetPName*/
	GL_TEXTURE_BINDING_1D				= 0x8068,/*# 1 I	# GetPName*/
	GL_TEXTURE_BINDING_2D				= 0x8069,/*# 1 I	# GetPName*/
	/* passthru: GetTextureParameter */
	GL_TEXTURE_WIDTH					= 0x1000,/*# GetTextureParameter*/
	GL_TEXTURE_HEIGHT					= 0x1001,/*# GetTextureParameter*/
	GL_TEXTURE_INTERNAL_FORMAT				= 0x1003,/*# GetTextureParameter*/
	GL_TEXTURE_BORDER_COLOR				= 0x1004,/*# GetTextureParameter*/
	GL_TEXTURE_RED_SIZE				= 0x805C,/*# GetTextureParameter*/
	GL_TEXTURE_GREEN_SIZE				= 0x805D,/*# GetTextureParameter*/
	GL_TEXTURE_BLUE_SIZE				= 0x805E,/*# GetTextureParameter*/
	GL_TEXTURE_ALPHA_SIZE				= 0x805F,/*# GetTextureParameter*/
	/* passthru: HintMode */
	GL_DONT_CARE					= 0x1100,/*# HintMode*/
	GL_FASTEST						= 0x1101,/*# HintMode*/
	GL_NICEST						= 0x1102,/*# HintMode*/
	/* passthru: DataType */
	GL_BYTE						= 0x1400,/*# DataType*/
	GL_UNSIGNED_BYTE					= 0x1401,/*# DataType*/
	GL_SHORT						= 0x1402,/*# DataType*/
	GL_UNSIGNED_SHORT					= 0x1403,/*# DataType*/
	GL_INT						= 0x1404,/*# DataType*/
	GL_UNSIGNED_INT					= 0x1405,/*# DataType*/
	GL_FLOAT						= 0x1406,/*# DataType*/
	GL_DOUBLE						= 0x140A,/*# DataType*/
/*# Deprecated in GL 3.0; undeprecated in GL 4.3 / KHR_debug*/
	/* passthru: ErrorCode */
	GL_STACK_OVERFLOW					= 0x0503,/*# ErrorCode*/
	GL_STACK_UNDERFLOW					= 0x0504,/*# ErrorCode*/
	/* passthru: LogicOp */
	GL_CLEAR						= 0x1500,/*# LogicOp*/
	GL_AND						= 0x1501,/*# LogicOp*/
	GL_AND_REVERSE					= 0x1502,/*# LogicOp*/
	GL_COPY						= 0x1503,/*# LogicOp*/
	GL_AND_INVERTED					= 0x1504,/*# LogicOp*/
	GL_NOOP						= 0x1505,/*# LogicOp*/
	GL_XOR						= 0x1506,/*# LogicOp*/
	GL_OR						= 0x1507,/*# LogicOp*/
	GL_NOR						= 0x1508,/*# LogicOp*/
	GL_EQUIV						= 0x1509,/*# LogicOp*/
	GL_INVERT						= 0x150A,/*# LogicOp*/
	GL_OR_REVERSE					= 0x150B,/*# LogicOp*/
	GL_COPY_INVERTED					= 0x150C,/*# LogicOp*/
	GL_OR_INVERTED					= 0x150D,/*# LogicOp*/
	GL_NAND						= 0x150E,/*# LogicOp*/
	GL_SET						= 0x150F,/*# LogicOp*/
	/* passthru: MatrixMode (for gl3.h, FBO attachment type) */
	GL_TEXTURE						= 0x1702,/*# MatrixMode*/
	/* passthru: PixelCopyType */
	GL_COLOR						= 0x1800,/*# PixelCopyType*/
	GL_DEPTH						= 0x1801,/*# PixelCopyType*/
	GL_STENCIL						= 0x1802,/*# PixelCopyType*/
	/* passthru: PixelFormat */
	GL_STENCIL_INDEX					= 0x1901,/*# PixelFormat*/
	GL_DEPTH_COMPONENT					= 0x1902,/*# PixelFormat*/
	GL_RED						= 0x1903,/*# PixelFormat*/
	GL_GREEN						= 0x1904,/*# PixelFormat*/
	GL_BLUE						= 0x1905,/*# PixelFormat*/
	GL_ALPHA						= 0x1906,/*# PixelFormat*/
	GL_RGB						= 0x1907,/*# PixelFormat*/
	GL_RGBA						= 0x1908,/*# PixelFormat*/
	/* passthru: PolygonMode */
	GL_POINT						= 0x1B00,/*# PolygonMode*/
	GL_LINE						= 0x1B01,/*# PolygonMode*/
	GL_FILL						= 0x1B02,/*# PolygonMode*/
	/* passthru: StencilOp */
	GL_KEEP						= 0x1E00,/*# StencilOp*/
	GL_REPLACE						= 0x1E01,/*# StencilOp*/
	GL_INCR						= 0x1E02,/*# StencilOp*/
	GL_DECR						= 0x1E03,/*# StencilOp*/
	/* passthru: StringName */
	GL_VENDOR						= 0x1F00,/*# StringName*/
	GL_RENDERER					= 0x1F01,/*# StringName*/
	GL_VERSION						= 0x1F02,/*# StringName*/
	GL_EXTENSIONS					= 0x1F03,/*# StringName*/
	/* passthru: TextureMagFilter */
	GL_NEAREST						= 0x2600,/*# TextureMagFilter*/
	GL_LINEAR						= 0x2601,/*# TextureMagFilter*/
	/* passthru: TextureMinFilter */
	GL_NEAREST_MIPMAP_NEAREST				= 0x2700,/*# TextureMinFilter*/
	GL_LINEAR_MIPMAP_NEAREST				= 0x2701,/*# TextureMinFilter*/
	GL_NEAREST_MIPMAP_LINEAR				= 0x2702,/*# TextureMinFilter*/
	GL_LINEAR_MIPMAP_LINEAR				= 0x2703,/*# TextureMinFilter*/
	/* passthru: TextureParameterName */
	GL_TEXTURE_MAG_FILTER				= 0x2800,/*# TextureParameterName*/
	GL_TEXTURE_MIN_FILTER				= 0x2801,/*# TextureParameterName*/
	GL_TEXTURE_WRAP_S					= 0x2802,/*# TextureParameterName*/
	GL_TEXTURE_WRAP_T					= 0x2803,/*# TextureParameterName*/
	/* passthru: TextureTarget */
	GL_PROXY_TEXTURE_1D				= 0x8063,/*# TextureTarget*/
	GL_PROXY_TEXTURE_2D				= 0x8064,/*# TextureTarget*/
	/* passthru: TextureWrapMode */
	GL_REPEAT						= 0x2901,/*# TextureWrapMode*/
	/* passthru: PixelInternalFormat */
	GL_R3_G3_B2					= 0x2A10,/*# PixelInternalFormat*/
	GL_RGB4						= 0x804F,/*# PixelInternalFormat*/
	GL_RGB5						= 0x8050,/*# PixelInternalFormat*/
	GL_RGB8						= 0x8051,/*# PixelInternalFormat*/
	GL_RGB10						= 0x8052,/*# PixelInternalFormat*/
	GL_RGB12						= 0x8053,/*# PixelInternalFormat*/
	GL_RGB16						= 0x8054,/*# PixelInternalFormat*/
	GL_RGBA2						= 0x8055,/*# PixelInternalFormat*/
	GL_RGBA4						= 0x8056,/*# PixelInternalFormat*/
	GL_RGB5_A1						= 0x8057,/*# PixelInternalFormat*/
	GL_RGBA8						= 0x8058,/*# PixelInternalFormat*/
	GL_RGB10_A2					= 0x8059,/*# PixelInternalFormat*/
	GL_RGBA12						= 0x805A,/*# PixelInternalFormat*/
	GL_RGBA16						= 0x805B,/*# PixelInternalFormat*/
/*profile: compatibility*/
	 /* passthru: AttribMask */
	 GL_CURRENT_BIT					= 0x00000001,/*# AttribMask*/
	 GL_POINT_BIT					= 0x00000002,/*# AttribMask*/
	 GL_LINE_BIT					= 0x00000004,/*# AttribMask*/
	 GL_POLYGON_BIT					= 0x00000008,/*# AttribMask*/
	 GL_POLYGON_STIPPLE_BIT				= 0x00000010,/*# AttribMask*/
	 GL_PIXEL_MODE_BIT					= 0x00000020,/*# AttribMask*/
	 GL_LIGHTING_BIT					= 0x00000040,/*# AttribMask*/
	 GL_FOG_BIT						= 0x00000080,/*# AttribMask*/
	 GL_ACCUM_BUFFER_BIT				= 0x00000200,/*# AttribMask*/
	 GL_VIEWPORT_BIT					= 0x00000800,/*# AttribMask*/
	 GL_TRANSFORM_BIT					= 0x00001000,/*# AttribMask*/
	 GL_ENABLE_BIT					= 0x00002000,/*# AttribMask*/
	 GL_HINT_BIT					= 0x00008000,/*# AttribMask*/
	 GL_EVAL_BIT					= 0x00010000,/*# AttribMask*/
	 GL_LIST_BIT					= 0x00020000,/*# AttribMask*/
	 GL_TEXTURE_BIT					= 0x00040000,/*# AttribMask*/
	 GL_SCISSOR_BIT					= 0x00080000,/*# AttribMask*/
	 GL_ALL_ATTRIB_BITS					= 0xFFFFFFFF,/*# AttribMask*/
	 /* passthru: ClientAttribMask */
	 GL_CLIENT_PIXEL_STORE_BIT				= 0x00000001,/*# ClientAttribMask*/
	 GL_CLIENT_VERTEX_ARRAY_BIT				= 0x00000002,/*# ClientAttribMask*/
	 GL_CLIENT_ALL_ATTRIB_BITS				= 0xFFFFFFFF,/*# ClientAttribMask*/
	 /* passthru: BeginMode */
	 GL_QUAD_STRIP					= 0x0008,/*# BeginMode*/
	 GL_POLYGON						= 0x0009,/*# BeginMode*/
	 /* passthru: AccumOp */
	 GL_ACCUM						= 0x0100,/*# AccumOp*/
	 GL_LOAD						= 0x0101,/*# AccumOp*/
	 GL_RETURN						= 0x0102,/*# AccumOp*/
	 GL_MULT						= 0x0103,/*# AccumOp*/
	 GL_ADD						= 0x0104,/*# AccumOp*/
	 /* passthru: DrawBufferMode */
	 GL_AUX0						= 0x0409,/*# DrawBufferMode*/
	 GL_AUX1						= 0x040A,/*# DrawBufferMode*/
	 GL_AUX2						= 0x040B,/*# DrawBufferMode*/
	 GL_AUX3						= 0x040C,/*# DrawBufferMode*/
	 /* passthru: FeedbackType */
	 GL_2D						= 0x0600,/*# FeedbackType*/
	 GL_3D						= 0x0601,/*# FeedbackType*/
	 GL_3D_COLOR					= 0x0602,/*# FeedbackType*/
	 GL_3D_COLOR_TEXTURE				= 0x0603,/*# FeedbackType*/
	 GL_4D_COLOR_TEXTURE				= 0x0604,/*# FeedbackType*/
	 /* passthru: FeedBackToken */
	 GL_PASS_THROUGH_TOKEN				= 0x0700,/*# FeedBackToken*/
	 GL_POINT_TOKEN					= 0x0701,/*# FeedBackToken*/
	 GL_LINE_TOKEN					= 0x0702,/*# FeedBackToken*/
	 GL_POLYGON_TOKEN					= 0x0703,/*# FeedBackToken*/
	 GL_BITMAP_TOKEN					= 0x0704,/*# FeedBackToken*/
	 GL_DRAW_PIXEL_TOKEN				= 0x0705,/*# FeedBackToken*/
	 GL_COPY_PIXEL_TOKEN				= 0x0706,/*# FeedBackToken*/
	 GL_LINE_RESET_TOKEN				= 0x0707,/*# FeedBackToken*/
	 /* passthru: FogMode */
	 GL_EXP						= 0x0800,/*# FogMode*/
	 GL_EXP2						= 0x0801,/*# FogMode*/
	 /* passthru: GetMapQuery */
	 GL_COEFF						= 0x0A00,/*# GetMapQuery*/
	 GL_ORDER						= 0x0A01,/*# GetMapQuery*/
	 GL_DOMAIN						= 0x0A02,/*# GetMapQuery*/
	 /* passthru: GetPixelMap */
	 GL_PIXEL_MAP_I_TO_I				= 0x0C70,/*# GetPixelMap*/
	 GL_PIXEL_MAP_S_TO_S				= 0x0C71,/*# GetPixelMap*/
	 GL_PIXEL_MAP_I_TO_R				= 0x0C72,/*# GetPixelMap*/
	 GL_PIXEL_MAP_I_TO_G				= 0x0C73,/*# GetPixelMap*/
	 GL_PIXEL_MAP_I_TO_B				= 0x0C74,/*# GetPixelMap*/
	 GL_PIXEL_MAP_I_TO_A				= 0x0C75,/*# GetPixelMap*/
	 GL_PIXEL_MAP_R_TO_R				= 0x0C76,/*# GetPixelMap*/
	 GL_PIXEL_MAP_G_TO_G				= 0x0C77,/*# GetPixelMap*/
	 GL_PIXEL_MAP_B_TO_B				= 0x0C78,/*# GetPixelMap*/
	 GL_PIXEL_MAP_A_TO_A				= 0x0C79,/*# GetPixelMap*/
	 /* passthru: GetPointervPName */
	 GL_VERTEX_ARRAY_POINTER				= 0x808E,/*# GetPointervPName*/
	 GL_NORMAL_ARRAY_POINTER				= 0x808F,/*# GetPointervPName*/
	 GL_COLOR_ARRAY_POINTER				= 0x8090,/*# GetPointervPName*/
	 GL_INDEX_ARRAY_POINTER				= 0x8091,/*# GetPointervPName*/
	 GL_TEXTURE_COORD_ARRAY_POINTER			= 0x8092,/*# GetPointervPName*/
	 GL_EDGE_FLAG_ARRAY_POINTER				= 0x8093,/*# GetPointervPName*/
	 GL_FEEDBACK_BUFFER_POINTER				= 0x0DF0,/*# GetPointervPName*/
	 GL_SELECTION_BUFFER_POINTER			= 0x0DF3,/*# GetPointervPName*/
	 /* passthru: GetPName */
	 GL_CURRENT_COLOR					= 0x0B00,/*# 4 F	# GetPName*/
	 GL_CURRENT_INDEX					= 0x0B01,/*# 1 F	# GetPName*/
	 GL_CURRENT_NORMAL					= 0x0B02,/*# 3 F	# GetPName*/
	 GL_CURRENT_TEXTURE_COORDS				= 0x0B03,/*# 4 F	# GetPName*/
	 GL_CURRENT_RASTER_COLOR				= 0x0B04,/*# 4 F	# GetPName*/
	 GL_CURRENT_RASTER_INDEX				= 0x0B05,/*# 1 F	# GetPName*/
	 GL_CURRENT_RASTER_TEXTURE_COORDS			= 0x0B06,/*# 4 F	# GetPName*/
	 GL_CURRENT_RASTER_POSITION				= 0x0B07,/*# 4 F	# GetPName*/
	 GL_CURRENT_RASTER_POSITION_VALID			= 0x0B08,/*# 1 I	# GetPName*/
	 GL_CURRENT_RASTER_DISTANCE				= 0x0B09,/*# 1 F	# GetPName*/
	 GL_POINT_SMOOTH					= 0x0B10,/*# 1 I	# GetPName*/
	 GL_LINE_STIPPLE					= 0x0B24,/*# 1 I	# GetPName*/
	 GL_LINE_STIPPLE_PATTERN				= 0x0B25,/*# 1 I	# GetPName*/
	 GL_LINE_STIPPLE_REPEAT				= 0x0B26,/*# 1 I	# GetPName*/
	 GL_LIST_MODE					= 0x0B30,/*# 1 I	# GetPName*/
	 GL_MAX_LIST_NESTING				= 0x0B31,/*# 1 I	# GetPName*/
	 GL_LIST_BASE					= 0x0B32,/*# 1 I	# GetPName*/
	 GL_LIST_INDEX					= 0x0B33,/*# 1 I	# GetPName*/
	 GL_POLYGON_STIPPLE					= 0x0B42,/*# 1 I	# GetPName*/
	 GL_EDGE_FLAG					= 0x0B43,/*# 1 I	# GetPName*/
	 GL_LIGHTING					= 0x0B50,/*# 1 I	# GetPName*/
	 GL_LIGHT_MODEL_LOCAL_VIEWER			= 0x0B51,/*# 1 I	# GetPName*/
	 GL_LIGHT_MODEL_TWO_SIDE				= 0x0B52,/*# 1 I	# GetPName*/
	 GL_LIGHT_MODEL_AMBIENT				= 0x0B53,/*# 4 F	# GetPName*/
	 GL_SHADE_MODEL					= 0x0B54,/*# 1 I	# GetPName*/
	 GL_COLOR_MATERIAL_FACE				= 0x0B55,/*# 1 I	# GetPName*/
	 GL_COLOR_MATERIAL_PARAMETER			= 0x0B56,/*# 1 I	# GetPName*/
	 GL_COLOR_MATERIAL					= 0x0B57,/*# 1 I	# GetPName*/
	 GL_FOG						= 0x0B60,/*# 1 I	# GetPName*/
	 GL_FOG_INDEX					= 0x0B61,/*# 1 I	# GetPName*/
	 GL_FOG_DENSITY					= 0x0B62,/*# 1 F	# GetPName*/
	 GL_FOG_START					= 0x0B63,/*# 1 F	# GetPName*/
	 GL_FOG_END						= 0x0B64,/*# 1 F	# GetPName*/
	 GL_FOG_MODE					= 0x0B65,/*# 1 I	# GetPName*/
	 GL_FOG_COLOR					= 0x0B66,/*# 4 F	# GetPName*/
	 GL_ACCUM_CLEAR_VALUE				= 0x0B80,/*# 4 F	# GetPName*/
	 GL_MATRIX_MODE					= 0x0BA0,/*# 1 I	# GetPName*/
	 GL_NORMALIZE					= 0x0BA1,/*# 1 I	# GetPName*/
	 GL_MODELVIEW_STACK_DEPTH				= 0x0BA3,/*# 1 I	# GetPName*/
	 GL_PROJECTION_STACK_DEPTH				= 0x0BA4,/*# 1 I	# GetPName*/
	 GL_TEXTURE_STACK_DEPTH				= 0x0BA5,/*# 1 I	# GetPName*/
	 GL_MODELVIEW_MATRIX				= 0x0BA6,/*# 16 F # GetPName*/
	 GL_PROJECTION_MATRIX				= 0x0BA7,/*# 16 F # GetPName*/
	 GL_TEXTURE_MATRIX					= 0x0BA8,/*# 16 F # GetPName*/
	 GL_ATTRIB_STACK_DEPTH				= 0x0BB0,/*# 1 I	# GetPName*/
	 GL_CLIENT_ATTRIB_STACK_DEPTH			= 0x0BB1,/*# 1 I	# GetPName*/
	 GL_ALPHA_TEST					= 0x0BC0,/*# 1 I	# GetPName*/
	 GL_ALPHA_TEST_FUNC					= 0x0BC1,/*# 1 I	# GetPName*/
	 GL_ALPHA_TEST_REF					= 0x0BC2,/*# 1 F	# GetPName*/
	 GL_INDEX_LOGIC_OP					= 0x0BF1,/*# 1 I	# GetPName*/
	 GL_LOGIC_OP					= 0x0BF1,/*# 1 I	# GetPName*/
	 GL_AUX_BUFFERS					= 0x0C00,/*# 1 I	# GetPName*/
	 GL_INDEX_CLEAR_VALUE				= 0x0C20,/*# 1 I	# GetPName*/
	 GL_INDEX_WRITEMASK					= 0x0C21,/*# 1 I	# GetPName*/
	 GL_INDEX_MODE					= 0x0C30,/*# 1 I	# GetPName*/
	 GL_RGBA_MODE					= 0x0C31,/*# 1 I	# GetPName*/
	 GL_RENDER_MODE					= 0x0C40,/*# 1 I	# GetPName*/
	 GL_PERSPECTIVE_CORRECTION_HINT			= 0x0C50,/*# 1 I	# GetPName*/
	 GL_POINT_SMOOTH_HINT				= 0x0C51,/*# 1 I	# GetPName*/
	 GL_FOG_HINT					= 0x0C54,/*# 1 I	# GetPName*/
	 GL_TEXTURE_GEN_S					= 0x0C60,/*# 1 I	# GetPName*/
	 GL_TEXTURE_GEN_T					= 0x0C61,/*# 1 I	# GetPName*/
	 GL_TEXTURE_GEN_R					= 0x0C62,/*# 1 I	# GetPName*/
	 GL_TEXTURE_GEN_Q					= 0x0C63,/*# 1 I	# GetPName*/
	 GL_PIXEL_MAP_I_TO_I_SIZE				= 0x0CB0,/*# 1 I	# GetPName*/
	 GL_PIXEL_MAP_S_TO_S_SIZE				= 0x0CB1,/*# 1 I	# GetPName*/
	 GL_PIXEL_MAP_I_TO_R_SIZE				= 0x0CB2,/*# 1 I	# GetPName*/
	 GL_PIXEL_MAP_I_TO_G_SIZE				= 0x0CB3,/*# 1 I	# GetPName*/
	 GL_PIXEL_MAP_I_TO_B_SIZE				= 0x0CB4,/*# 1 I	# GetPName*/
	 GL_PIXEL_MAP_I_TO_A_SIZE				= 0x0CB5,/*# 1 I	# GetPName*/
	 GL_PIXEL_MAP_R_TO_R_SIZE				= 0x0CB6,/*# 1 I	# GetPName*/
	 GL_PIXEL_MAP_G_TO_G_SIZE				= 0x0CB7,/*# 1 I	# GetPName*/
	 GL_PIXEL_MAP_B_TO_B_SIZE				= 0x0CB8,/*# 1 I	# GetPName*/
	 GL_PIXEL_MAP_A_TO_A_SIZE				= 0x0CB9,/*# 1 I	# GetPName*/
	 GL_MAP_COLOR					= 0x0D10,/*# 1 I	# GetPName*/
	 GL_MAP_STENCIL					= 0x0D11,/*# 1 I	# GetPName*/
	 GL_INDEX_SHIFT					= 0x0D12,/*# 1 I	# GetPName*/
	 GL_INDEX_OFFSET					= 0x0D13,/*# 1 I	# GetPName*/
	 GL_RED_SCALE					= 0x0D14,/*# 1 F	# GetPName*/
	 GL_RED_BIAS					= 0x0D15,/*# 1 F	# GetPName*/
	 GL_ZOOM_X						= 0x0D16,/*# 1 F	# GetPName*/
	 GL_ZOOM_Y						= 0x0D17,/*# 1 F	# GetPName*/
	 GL_GREEN_SCALE					= 0x0D18,/*# 1 F	# GetPName*/
	 GL_GREEN_BIAS					= 0x0D19,/*# 1 F	# GetPName*/
	 GL_BLUE_SCALE					= 0x0D1A,/*# 1 F	# GetPName*/
	 GL_BLUE_BIAS					= 0x0D1B,/*# 1 F	# GetPName*/
	 GL_ALPHA_SCALE					= 0x0D1C,/*# 1 F	# GetPName*/
	 GL_ALPHA_BIAS					= 0x0D1D,/*# 1 F	# GetPName*/
	 GL_DEPTH_SCALE					= 0x0D1E,/*# 1 F	# GetPName*/
	 GL_DEPTH_BIAS					= 0x0D1F,/*# 1 F	# GetPName*/
	 GL_MAX_EVAL_ORDER					= 0x0D30,/*# 1 I	# GetPName*/
	 GL_MAX_LIGHTS					= 0x0D31,/*# 1 I	# GetPName*/
	 GL_MAX_CLIP_PLANES					= 0x0D32,/*# 1 I	# GetPName*/
	 GL_MAX_PIXEL_MAP_TABLE				= 0x0D34,/*# 1 I	# GetPName*/
	 GL_MAX_ATTRIB_STACK_DEPTH				= 0x0D35,/*# 1 I	# GetPName*/
	 GL_MAX_MODELVIEW_STACK_DEPTH			= 0x0D36,/*# 1 I	# GetPName*/
	 GL_MAX_NAME_STACK_DEPTH				= 0x0D37,/*# 1 I	# GetPName*/
	 GL_MAX_PROJECTION_STACK_DEPTH			= 0x0D38,/*# 1 I	# GetPName*/
	 GL_MAX_TEXTURE_STACK_DEPTH				= 0x0D39,/*# 1 I	# GetPName*/
	 GL_MAX_CLIENT_ATTRIB_STACK_DEPTH			= 0x0D3B,/*# 1 I	# GetPName*/
	 GL_INDEX_BITS					= 0x0D51,/*# 1 I	# GetPName*/
	 GL_RED_BITS					= 0x0D52,/*# 1 I	# GetPName*/
	 GL_GREEN_BITS					= 0x0D53,/*# 1 I	# GetPName*/
	 GL_BLUE_BITS					= 0x0D54,/*# 1 I	# GetPName*/
	 GL_ALPHA_BITS					= 0x0D55,/*# 1 I	# GetPName*/
	 GL_DEPTH_BITS					= 0x0D56,/*# 1 I	# GetPName*/
	 GL_STENCIL_BITS					= 0x0D57,/*# 1 I	# GetPName*/
	 GL_ACCUM_RED_BITS					= 0x0D58,/*# 1 I	# GetPName*/
	 GL_ACCUM_GREEN_BITS				= 0x0D59,/*# 1 I	# GetPName*/
	 GL_ACCUM_BLUE_BITS					= 0x0D5A,/*# 1 I	# GetPName*/
	 GL_ACCUM_ALPHA_BITS				= 0x0D5B,/*# 1 I	# GetPName*/
	 GL_NAME_STACK_DEPTH				= 0x0D70,/*# 1 I	# GetPName*/
	 GL_AUTO_NORMAL					= 0x0D80,/*# 1 I	# GetPName*/
	 GL_MAP1_COLOR_4					= 0x0D90,/*# 1 I	# GetPName*/
	 GL_MAP1_INDEX					= 0x0D91,/*# 1 I	# GetPName*/
	 GL_MAP1_NORMAL					= 0x0D92,/*# 1 I	# GetPName*/
	 GL_MAP1_TEXTURE_COORD_1				= 0x0D93,/*# 1 I	# GetPName*/
	 GL_MAP1_TEXTURE_COORD_2				= 0x0D94,/*# 1 I	# GetPName*/
	 GL_MAP1_TEXTURE_COORD_3				= 0x0D95,/*# 1 I	# GetPName*/
	 GL_MAP1_TEXTURE_COORD_4				= 0x0D96,/*# 1 I	# GetPName*/
	 GL_MAP1_VERTEX_3					= 0x0D97,/*# 1 I	# GetPName*/
	 GL_MAP1_VERTEX_4					= 0x0D98,/*# 1 I	# GetPName*/
	 GL_MAP2_COLOR_4					= 0x0DB0,/*# 1 I	# GetPName*/
	 GL_MAP2_INDEX					= 0x0DB1,/*# 1 I	# GetPName*/
	 GL_MAP2_NORMAL					= 0x0DB2,/*# 1 I	# GetPName*/
	 GL_MAP2_TEXTURE_COORD_1				= 0x0DB3,/*# 1 I	# GetPName*/
	 GL_MAP2_TEXTURE_COORD_2				= 0x0DB4,/*# 1 I	# GetPName*/
	 GL_MAP2_TEXTURE_COORD_3				= 0x0DB5,/*# 1 I	# GetPName*/
	 GL_MAP2_TEXTURE_COORD_4				= 0x0DB6,/*# 1 I	# GetPName*/
	 GL_MAP2_VERTEX_3					= 0x0DB7,/*# 1 I	# GetPName*/
	 GL_MAP2_VERTEX_4					= 0x0DB8,/*# 1 I	# GetPName*/
	 GL_MAP1_GRID_DOMAIN				= 0x0DD0,/*# 2 F	# GetPName*/
	 GL_MAP1_GRID_SEGMENTS				= 0x0DD1,/*# 1 I	# GetPName*/
	 GL_MAP2_GRID_DOMAIN				= 0x0DD2,/*# 4 F	# GetPName*/
	 GL_MAP2_GRID_SEGMENTS				= 0x0DD3,/*# 2 I	# GetPName*/
	 GL_FEEDBACK_BUFFER_SIZE				= 0x0DF1,/*# 1 I	# GetPName*/
	 GL_FEEDBACK_BUFFER_TYPE				= 0x0DF2,/*# 1 I	# GetPName*/
	 GL_SELECTION_BUFFER_SIZE				= 0x0DF4,/*# 1 I	# GetPName*/
	 GL_VERTEX_ARRAY					= 0x8074,/*# 1 I	# GetPName*/
	 GL_NORMAL_ARRAY					= 0x8075,/*# 1 I	# GetPName*/
	 GL_COLOR_ARRAY					= 0x8076,/*# 1 I	# GetPName*/
	 GL_INDEX_ARRAY					= 0x8077,/*# 1 I	# GetPName*/
	 GL_TEXTURE_COORD_ARRAY				= 0x8078,/*# 1 I	# GetPName*/
	 GL_EDGE_FLAG_ARRAY					= 0x8079,/*# 1 I	# GetPName*/
	 GL_VERTEX_ARRAY_SIZE				= 0x807A,/*# 1 I	# GetPName*/
	 GL_VERTEX_ARRAY_TYPE				= 0x807B,/*# 1 I	# GetPName*/
	 GL_VERTEX_ARRAY_STRIDE				= 0x807C,/*# 1 I	# GetPName*/
	 GL_NORMAL_ARRAY_TYPE				= 0x807E,/*# 1 I	# GetPName*/
	 GL_NORMAL_ARRAY_STRIDE				= 0x807F,/*# 1 I	# GetPName*/
	 GL_COLOR_ARRAY_SIZE				= 0x8081,/*# 1 I	# GetPName*/
	 GL_COLOR_ARRAY_TYPE				= 0x8082,/*# 1 I	# GetPName*/
	 GL_COLOR_ARRAY_STRIDE				= 0x8083,/*# 1 I	# GetPName*/
	 GL_INDEX_ARRAY_TYPE				= 0x8085,/*# 1 I	# GetPName*/
	 GL_INDEX_ARRAY_STRIDE				= 0x8086,/*# 1 I	# GetPName*/
	 GL_TEXTURE_COORD_ARRAY_SIZE			= 0x8088,/*# 1 I	# GetPName*/
	 GL_TEXTURE_COORD_ARRAY_TYPE			= 0x8089,/*# 1 I	# GetPName*/
	 GL_TEXTURE_COORD_ARRAY_STRIDE			= 0x808A,/*# 1 I	# GetPName*/
	 GL_EDGE_FLAG_ARRAY_STRIDE				= 0x808C,/*# 1 I	# GetPName*/
	 /* passthru: GetTextureParameter */
	 GL_TEXTURE_COMPONENTS				= 0x1003,/*# GetTextureParameter*/
	 GL_TEXTURE_BORDER					= 0x1005,/*# GetTextureParameter*/
	 GL_TEXTURE_LUMINANCE_SIZE				= 0x8060,/*# GetTextureParameter*/
	 GL_TEXTURE_INTENSITY_SIZE				= 0x8061,/*# GetTextureParameter*/
	 GL_TEXTURE_PRIORITY				= 0x8066,/*# GetTextureParameter*/
	 GL_TEXTURE_RESIDENT				= 0x8067,/*# GetTextureParameter*/
	 /* passthru: LightParameter */
	 GL_AMBIENT						= 0x1200,/*# LightParameter*/
	 GL_DIFFUSE						= 0x1201,/*# LightParameter*/
	 GL_SPECULAR					= 0x1202,/*# LightParameter*/
	 GL_POSITION					= 0x1203,/*# LightParameter*/
	 GL_SPOT_DIRECTION					= 0x1204,/*# LightParameter*/
	 GL_SPOT_EXPONENT					= 0x1205,/*# LightParameter*/
	 GL_SPOT_CUTOFF					= 0x1206,/*# LightParameter*/
	 GL_CONSTANT_ATTENUATION				= 0x1207,/*# LightParameter*/
	 GL_LINEAR_ATTENUATION				= 0x1208,/*# LightParameter*/
	 GL_QUADRATIC_ATTENUATION				= 0x1209,/*# LightParameter*/
	 /* passthru: ListMode */
	 GL_COMPILE						= 0x1300,/*# ListMode*/
	 GL_COMPILE_AND_EXECUTE				= 0x1301,/*# ListMode*/
	 /* passthru: DataType */
	 GL_2_BYTES						= 0x1407,/*# DataType*/
	 GL_3_BYTES						= 0x1408,/*# DataType*/
	 GL_4_BYTES						= 0x1409,/*# DataType*/
	 /* passthru: MaterialParameter */
	 GL_EMISSION					= 0x1600,/*# MaterialParameter*/
	 GL_SHININESS					= 0x1601,/*# MaterialParameter*/
	 GL_AMBIENT_AND_DIFFUSE				= 0x1602,/*# MaterialParameter*/
	 GL_COLOR_INDEXES					= 0x1603,/*# MaterialParameter*/
	 /* passthru: MatrixMode */
	 GL_MODELVIEW					= 0x1700,/*# MatrixMode*/
	 GL_PROJECTION					= 0x1701,/*# MatrixMode*/
	 /* passthru: PixelFormat */
	 GL_COLOR_INDEX					= 0x1900,/*# PixelFormat*/
	 GL_LUMINANCE					= 0x1909,/*# PixelFormat*/
	 GL_LUMINANCE_ALPHA					= 0x190A,/*# PixelFormat*/
	 /* passthru: PixelType */
	 GL_BITMAP						= 0x1A00,/*# PixelType*/
	 /* passthru: RenderingMode */
	 GL_RENDER						= 0x1C00,/*# RenderingMode*/
	 GL_FEEDBACK					= 0x1C01,/*# RenderingMode*/
	 GL_SELECT						= 0x1C02,/*# RenderingMode*/
	 /* passthru: ShadingModel */
	 GL_FLAT						= 0x1D00,/*# ShadingModel*/
	 GL_SMOOTH						= 0x1D01,/*# ShadingModel*/
	 /* passthru: TextureCoordName */
	 GL_S						= 0x2000,/*# TextureCoordName*/
	 GL_T						= 0x2001,/*# TextureCoordName*/
	 GL_R						= 0x2002,/*# TextureCoordName*/
	 GL_Q						= 0x2003,/*# TextureCoordName*/
	 /* passthru: TextureEnvMode */
	 GL_MODULATE					= 0x2100,/*# TextureEnvMode*/
	 GL_DECAL						= 0x2101,/*# TextureEnvMode*/
	 /* passthru: TextureEnvParameter */
	 GL_TEXTURE_ENV_MODE				= 0x2200,/*# TextureEnvParameter*/
	 GL_TEXTURE_ENV_COLOR				= 0x2201,/*# TextureEnvParameter*/
	 /* passthru: TextureEnvTarget */
	 GL_TEXTURE_ENV					= 0x2300,/*# TextureEnvTarget*/
	 /* passthru: TextureGenMode */
	 GL_EYE_LINEAR					= 0x2400,/*# TextureGenMode*/
	 GL_OBJECT_LINEAR					= 0x2401,/*# TextureGenMode*/
	 GL_SPHERE_MAP					= 0x2402,/*# TextureGenMode*/
	 /* passthru: TextureGenParameter */
	 GL_TEXTURE_GEN_MODE				= 0x2500,/*# TextureGenParameter*/
	 GL_OBJECT_PLANE					= 0x2501,/*# TextureGenParameter*/
	 GL_EYE_PLANE					= 0x2502,/*# TextureGenParameter*/
	 /* passthru: TextureWrapMode */
	 GL_CLAMP						= 0x2900,/*# TextureWrapMode*/
	 /* passthru: PixelInternalFormat */
	 GL_ALPHA4						= 0x803B,/*# PixelInternalFormat*/
	 GL_ALPHA8						= 0x803C,/*# PixelInternalFormat*/
	 GL_ALPHA12						= 0x803D,/*# PixelInternalFormat*/
	 GL_ALPHA16						= 0x803E,/*# PixelInternalFormat*/
	 GL_LUMINANCE4					= 0x803F,/*# PixelInternalFormat*/
	 GL_LUMINANCE8					= 0x8040,/*# PixelInternalFormat*/
	 GL_LUMINANCE12					= 0x8041,/*# PixelInternalFormat*/
	 GL_LUMINANCE16					= 0x8042,/*# PixelInternalFormat*/
	 GL_LUMINANCE4_ALPHA4				= 0x8043,/*# PixelInternalFormat*/
	 GL_LUMINANCE6_ALPHA2				= 0x8044,/*# PixelInternalFormat*/
	 GL_LUMINANCE8_ALPHA8				= 0x8045,/*# PixelInternalFormat*/
	 GL_LUMINANCE12_ALPHA4				= 0x8046,/*# PixelInternalFormat*/
	 GL_LUMINANCE12_ALPHA12				= 0x8047,/*# PixelInternalFormat*/
	 GL_LUMINANCE16_ALPHA16				= 0x8048,/*# PixelInternalFormat*/
	 GL_INTENSITY					= 0x8049,/*# PixelInternalFormat*/
	 GL_INTENSITY4					= 0x804A,/*# PixelInternalFormat*/
	 GL_INTENSITY8					= 0x804B,/*# PixelInternalFormat*/
	 GL_INTENSITY12					= 0x804C,/*# PixelInternalFormat*/
	 GL_INTENSITY16					= 0x804D,/*# PixelInternalFormat*/
	 /* passthru: InterleavedArrayFormat */
	 GL_V2F						= 0x2A20,/*# InterleavedArrayFormat*/
	 GL_V3F						= 0x2A21,/*# InterleavedArrayFormat*/
	 GL_C4UB_V2F					= 0x2A22,/*# InterleavedArrayFormat*/
	 GL_C4UB_V3F					= 0x2A23,/*# InterleavedArrayFormat*/
	 GL_C3F_V3F						= 0x2A24,/*# InterleavedArrayFormat*/
	 GL_N3F_V3F						= 0x2A25,/*# InterleavedArrayFormat*/
	 GL_C4F_N3F_V3F					= 0x2A26,/*# InterleavedArrayFormat*/
	 GL_T2F_V3F						= 0x2A27,/*# InterleavedArrayFormat*/
	 GL_T4F_V4F						= 0x2A28,/*# InterleavedArrayFormat*/
	 GL_T2F_C4UB_V3F					= 0x2A29,/*# InterleavedArrayFormat*/
	 GL_T2F_C3F_V3F					= 0x2A2A,/*# InterleavedArrayFormat*/
	 GL_T2F_N3F_V3F					= 0x2A2B,/*# InterleavedArrayFormat*/
	 GL_T2F_C4F_N3F_V3F					= 0x2A2C,/*# InterleavedArrayFormat*/
	 GL_T4F_C4F_N3F_V4F					= 0x2A2D,/*# InterleavedArrayFormat*/
	 /* passthru: ClipPlaneName */
	 GL_CLIP_PLANE0					= 0x3000,/*# 1 I	# ClipPlaneName*/
	 GL_CLIP_PLANE1					= 0x3001,/*# 1 I	# ClipPlaneName*/
	 GL_CLIP_PLANE2					= 0x3002,/*# 1 I	# ClipPlaneName*/
	 GL_CLIP_PLANE3					= 0x3003,/*# 1 I	# ClipPlaneName*/
	 GL_CLIP_PLANE4					= 0x3004,/*# 1 I	# ClipPlaneName*/
	 GL_CLIP_PLANE5					= 0x3005,/*# 1 I	# ClipPlaneName*/
	 /* passthru: LightName */
	 GL_LIGHT0						= 0x4000,/*# 1 I	# LightName*/
	 GL_LIGHT1						= 0x4001,/*# 1 I	# LightName*/
	 GL_LIGHT2						= 0x4002,/*# 1 I	# LightName*/
	 GL_LIGHT3						= 0x4003,/*# 1 I	# LightName*/
	 GL_LIGHT4						= 0x4004,/*# 1 I	# LightName*/
	 GL_LIGHT5						= 0x4005,/*# 1 I	# LightName*/
	 GL_LIGHT6						= 0x4006,/*# 1 I	# LightName*/
	 GL_LIGHT7						= 0x4007,/*# 1 I	# LightName*/
};
/*
// has param but no number
#define DECLARE_GLFUNC_0(name,type,full_type) void gl##name##type##(full_type x);
#define DECLARE_GLFUNC_1(name,type,full_type) void gl##name##1##type##(full_type x);
#define DECLARE_GLFUNC_2(name,type,full_type) void gl##name##2##type##(full_type x,full_type y);
#define DECLARE_GLFUNC_3(name,type,full_type) void gl##name##3##type##(full_type x,full_type y,full_type z);
#define DECLARE_GLFUNC_4(name,type,full_type) void gl##name##4##type##(full_type x,full_type y,full_type z,full_type w);
#define DECLARE_GLFUNC_V0(name,type,full_type) void gl##name##type##(full_type* v);
#define DECLARE_GLFUNC_V1(name,type,full_type) void gl##name##1##type##(full_type* v);
#define DECLARE_GLFUNC_V2(name,type,full_type) void gl##name##2##type##v(full_type* v);
#define DECLARE_GLFUNC_V3(name,type,full_type) void gl##name##3##type##v(full_type* v);
#define DECLARE_GLFUNC_V4(name,type,full_type) void gl##name##4##type##v(full_type* v);

#define DECLARE_GLFUNC_0V(name,type,full_type) DECLARE_GLFUNC_0(name,type,full_type)\
	DECLARE_GLFUNC_V0(name,type,full_type)
#define DECLARE_GLFUNC_1V(name,type,full_type) DECLARE_GLFUNC_1(name,type,full_type)\
	DECLARE_GLFUNC_V1(name,type,full_type)
#define DECLARE_GLFUNC_1V(name,type,full_type) DECLARE_GLFUNC_1(name,type,full_type)\
	DECLARE_GLFUNC_V1(name,type,full_type)
#define DECLARE_GLFUNC_2V(name,type,full_type) DECLARE_GLFUNC_2(name,type,full_type)\
	DECLARE_GLFUNC_V2(name,type,full_type)
#define DECLARE_GLFUNC_3V(name,type,full_type) DECLARE_GLFUNC_3(name,type,full_type)\
	DECLARE_GLFUNC_V3(name,type,full_type)
#define DECLARE_GLFUNC_4V(name,type,full_type) DECLARE_GLFUNC_4(name,type,full_type)\
	DECLARE_GLFUNC_V4(name,type,full_type)


// #define DECLARE_GLFUNC_234V(name,type,full_type) DECLARE_GLFUNC_2(name,type,full_type)\
// 	DECLARE_GLFUNC_3(name,type,full_type)\
// 
// #define DECLARE_GLFUNC_234V(name,type,full_type) DECLARE_GLFUNC_2(name,type,full_type)\
// 	DECLARE_GLFUNC_3(name,type,full_type)\
// 	DECLARE_GLFUNC_4(name,type,full_type)\
// 	DECLARE_GLFUNC_2V(name,type,full_type)\
// 	DECLARE_GLFUNC_3V(name,type,full_type)\
// 	DECLARE_GLFUNC_4V(name,type,full_type)

#define DECLARE_GLFUNC_1V_bsifd(name) \
	DECLARE_GLFUNC_1V(name,b,GLbyte)
	DECLARE_GLFUNC_1V(name,s,GLshort)\
	DECLARE_GLFUNC_1V(name,i,GLint)\
	DECLARE_GLFUNC_1V(name,f,GLfloat)\
	DECLARE_GLFUNC_1V(name,d,GLdouble)

#define DECLARE_GLFUNC_1V_bsifdu(name) \
	DECLARE_GLFUNC_1V_bsifd(name)\
	DECLARE_GLFUNC_1V(name,ub,GLubyte)
	DECLARE_GLFUNC_1V(name,us,GLushort)\
	DECLARE_GLFUNC_1V(name,ui,GLuint)


#define DECLARE_GLFUNC_234V_bsifd(name) \
	DECLARE_GLFUNC_234V(name,f,GLfloat)\
	DECLARE_GLFUNC_234V(name,d,GLdouble)\
	DECLARE_GLFUNC_234V(name,s,GLshort)\
	DECLARE_GLFUNC_234V(name,i,GLint)\
	DECLARE_GLFUNC_234V(name,us,GLushort)\
	DECLARE_GLFUNC_234V(name,ui,GLuint)



DECLARE_GLFUNC_234V_bsifd(Vertex)
DECLARE_GLFUNC_234V_bsifd(Color)
DECLARE_GLFUNC_234V_bsifd(Normal)*/



void glEnable(GLenum cap);
void glDisable(GLenum cap);

void glBegin(GLenum mode);
void glEnd(void);

/* vertex data */

void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
// #define glVertex4fv(v) glVertex4f((v)[0],(v)[1],(v)[2],(v)[3])
// #define glVertex2f(x,y) glVertex4f(x,y,0.f,1.f)
// #define glVertex3f(x,y,z) glVertex4f(x,y,z,1.f)
// #define glVertex2fv(v) glVertex4f((v)[0],(v)[1],0.f,1.f)
// #define glVertex3fv(v) glVertex4f((v)[0],(v)[1],(v)[2],1.f)

inline void glVertex4fv(const GLfloat* v){ glVertex4f(v[0],v[1],v[2],v[3]); }
inline void glVertex2f(GLfloat x, GLfloat y){ glVertex4f(x,y,0.f,1.f); }
inline void glVertex3f(GLfloat x, GLfloat y, GLfloat z){ glVertex4f(x,y,z,1.f); }
inline void glVertex2fv(const GLfloat* v){ glVertex4f(v[0],v[1],0.f,1.f); }
inline void glVertex3fv(const GLfloat* v){ glVertex4f(v[0],v[1],v[2],1.f); }

void glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
// #define glColor4fv(v) glColor4f((v)[0],(v)[1],(v)[2],(v)[3])
// #define glColor3f(r,g,b) glColor4f(r,g,b,1.f)
// #define glColor3fv(v) glColor4f((v)[0],(v)[1],(v)[2],1.f)
inline void glColor3f(GLfloat r,GLfloat g,GLfloat b){ glColor4f(r,g,b,1.f); }
inline void glColor3fv(const GLfloat* v){ glColor4f(v[0],v[1],v[2],1.f); }
inline void glColor4fv(const GLfloat *v){ glColor4f(v[0],v[1],v[2],v[3]); }

void glSecondaryColor3f(GLfloat red, GLfloat green, GLfloat blue/*, GLfloat alpha = 0.f*/);
//#define glSecondaryColor3fv(v) glSecondaryColor3f((v)[0],(v)[1],(v)[2])
inline void glSecondaryColor3fv(const GLfloat* v){ glSecondaryColor3f(v[0],v[1],v[2]); }

void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
//#define glNormal3fv(v) glNormal3f((v)[0],(v)[1],(v)[2])
inline void glNormal3fv(const GLfloat* v){ glNormal3f(v[0],v[1],v[2]); }

void glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
// #define glTexCoord4fv(v) glTexCoord4f((v)[0],(v)[1],(v)[2],(v)[3])
// #define glTexCoord1f(s) glTexCoord4f(s,0.f,0.f,1.f)
// #define glTexCoord2f(s,t) glTexCoord4f(s,t,0.f,1.f)
// #define glTexCoord3f(s,t,r) glTexCoord4f(s,t,r,1.f)
// #define glTexCoord1fv(v) glTexCoord4f((v)[0],0.f,0.f,1.f)
// #define glTexCoord2fv(v) glTexCoord4f((v)[0],(v)[1],0.f,1.f)
// #define glTexCoord3fv(v) glTexCoord4f((v)[0],(v)[1],(v)[2],1.f)
inline void glTexCoord4fv(const GLfloat* v){ glTexCoord4f(v[0],v[1],v[2],v[3]); }
inline void glTexCoord1f(GLfloat s){ glTexCoord4f(s,0.f,0.f,1.f); }
inline void glTexCoord2f(GLfloat s, GLfloat t){ glTexCoord4f(s,t,0.f,1.f); }
inline void glTexCoord3f(GLfloat s, GLfloat t, GLfloat r){ glTexCoord4f(s,t,r,1.f); }
inline void glTexCoord1fv(const GLfloat* v){ glTexCoord4f(v[0],0.f,0.f,1.f); }
inline void glTexCoord2fv(const GLfloat* v){ glTexCoord4f(v[0],v[1],0.f,1.f); }
inline void glTexCoord3fv(const GLfloat* v){ glTexCoord4f(v[0],v[1],v[2],1.f); }

void glMultiTexCoord4f(GLenum tex, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
// #define glMultiTexCoord4fv(v) glMultiTexCoord4f((v)[0],(v)[1],(v)[2],(v)[3])
// #define glMultiTexCoord1f(tex,s) glMultiTexCoord4f(tex,s,0.f,0.f,1.f)
// #define glMultiTexCoord2f(tex,s,t) glMultiTexCoord4f(tex,s,t,0.f,1.f)
// #define glMultiTexCoord3f(tex,s,t,r) glMultiTexCoord4f(tex,s,t,r,1.f)
// #define glMultiTexCoord1fv(tex,v) glMultiTexCoord4f(tex,(v)[0],0.f,0.f,1.f)
// #define glMultiTexCoord2fv(tex,v) glMultiTexCoord4f(tex,(v)[0],(v)[1],0.f,1.f)
// #define glMultiTexCoord3fv(tex,v) glMultiTexCoord4f(tex,(v)[0],(v)[1],(v)[2],1.f)
inline void glMultiTexCoord4fv(GLenum tex, const GLfloat* v){ glMultiTexCoord4f(tex,v[0],v[1],v[2],v[3]); }
inline void glMultiTexCoord1f(GLenum tex, GLfloat s){ glMultiTexCoord4f(tex,s,0.f,0.f,1.f); }
inline void glMultiTexCoord2f(GLenum tex, GLfloat s, GLfloat t){ glMultiTexCoord4f(tex,s,t,0.f,1.f); }
inline void glMultiTexCoord3f(GLenum tex, GLfloat s, GLfloat t, GLfloat r){ glMultiTexCoord4f(tex,s,t,r,1.f); }
inline void glMultiTexCoord1fv(GLenum tex, const GLfloat*v){ glMultiTexCoord4f(tex,v[0],0.f,0.f,1.f); }
inline void glMultiTexCoord2fv(GLenum tex, const GLfloat*v){ glMultiTexCoord4f(tex,v[0],v[1],0.f,1.f); }
inline void glMultiTexCoord3fv(GLenum tex, const GLfloat*v){ glMultiTexCoord4f(tex,v[0],v[1],v[2],1.f); }

// void glIndexf(GLfloat c);

// void glFogCoordf(GLfloat f);
// #define glFogCoordfv(v) glFogCoordf(*(v))

void glEdgeFlag(GLboolean flag);

void glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
// #define glRasterPos4fv(v) glRasterPos4f((v)[0],(v)[1],(v)[2],(v)[3])
// #define glRasterPos2f(x,y,z) glRasterPos4f(x,y,0.f,1.f)
// #define glRasterPos3f(x,y,z) glRasterPos4f(x,y,z,1.f)
// #define glRasterPos2fv(v) glRasterPos4f(v[0],v[1],0.f,1.f)
// #define glRasterPos3fv(v) glRasterPos4f(v[0],v[1],v[2],1.f)
inline void glRasterPos4fv(const GLfloat* v){ glRasterPos4f(v[0],v[1],v[2],v[3]); }
inline void glRasterPos2f(GLfloat x, GLfloat y, GLfloat z){ glRasterPos4f(x,y,0.f,1.f); }
inline void glRasterPos3f(GLfloat x, GLfloat y, GLfloat z){ glRasterPos4f(x,y,z,1.f); }
inline void glRasterPos2fv(const GLfloat* v){ glRasterPos4f(v[0],v[1],0.f,1.f); }
inline void glRasterPos3fv(const GLfloat* v){ glRasterPos4f(v[0],v[1],v[2],1.f); }

void glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
// #define glRectfv(v1,v2) glRectf((v1)[0],(v1)[1],(v2)[0],(v2)[1])
inline void glRectfv(const GLfloat* v1,const GLfloat* v2){ glRectf(v1[0],v1[1],v2[0],v2[1]); }

/* lighting */

void glMaterialf(GLenum face, GLenum pname, GLfloat param);
void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params);
void glLightf(GLenum light, GLenum pname, GLfloat param);
void glLightfv(GLenum light, GLenum pname, const GLfloat *params);
void glLightModeli(GLenum pname, GLint param);
void glLightModeliv(GLenum pname, const GLint *params);
void glColorMaterial(GLenum face, GLenum mode);

// low-left (x,y)
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

/* projection */
void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);

/* MatrixMode */
#define GL_MODELVIEW                      0x1700
#define GL_PROJECTION                     0x1701
#define GL_TEXTURE                        0x1702
/* matrix */
void glMatrixMode(GLenum mode);
void glLoadIdentity(void);
void glPushMatrix(void);
void glPopMatrix(void);
void glLoadMatrixf(const GLfloat *m);
void glMultMatrixf(const GLfloat *m);
void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
void glScalef(GLfloat x, GLfloat y, GLfloat z);

/*	
	todo:
	vertex array
*/

/*	
	todo:
	buffer objects
*/

void glFrontFace(GLenum mode);
void glShadeModel(GLenum mode);
void glCullFace(GLenum mode);
void glDepthRange(GLclampd n, GLclampd f);

void glClear(GLbitfield mask);
void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void glFlush();
void glFinish();

/* texture */


void glTexGenf(GLenum coord, GLenum pname, GLfloat param);
void glTexGenfv(GLenum coord, GLenum pname, const GLfloat *params);

}//end namespace

#endif
#define WIN_COLOR_MAX		7
#define WINDOW_MAX		16

#define WIN_MIN_X		32
#define	WIN_MIN_Y		32

#define MOUSE_LEFT_LEVEL	0x0001
#define MOUSE_MIDDLE_LEVEL	0x0002
#define MOUSE_RIGHT_LEVEL	0x0004
#define MOUSE_LEFT_DOWN		0x0008
#define MOUSE_MIDDLE_DOWN	0x0010
#define MOUSE_RIGHT_DOWN	0x0020
#define MOUSE_LEFT_UP		0x0040
#define MOUSE_MIDDLE_UP		0x0080
#define MOUSE_RIGHT_UP		0x0100
#define MOUSE_LEVEL		0x0007
#define MOUSE_DOWN		0x0038
#define MOUSE_UP		0x01c0
#define MOUSE_LAUNCH		0x0200

#define LOCATION_CENTER		-1
#define LOCATION_AUTO		-2

#define PART_POSITIVE_BOX	0
#define PART_NEGATIVE_BOX	1
#define PART_IMAGE_BOX		2
#define PART_FRAME_BOX		3
#define PART_HANDLE_BASE	4
#define PART_FRAME_BASE		5
#define PART_PUSH_SWITCH	6
#define PART_TOUCH_SWITCH	7
#define PART_TEXT_CHOOSER	8
#define PART_TEXT_BOX		9
#define PART_VERTICAL_SLIDER	10
#define PART_HORIZONTAL_SLIDER	11
#define PART_ICON_TEXT		12
#define PART_ICON_IMAGE		13

#define SLIDER_UP_PART		0
#define SLIDER_DOWN_PART	1
#define SLIDER_RIGHT_PART	2
#define SLIDER_LEFT_PART	3

#define BASE_WITH_RESIZE	0x00000002
#define BASE_WITH_MAX		0x00000004
#define BASE_WITH_CLOSE		0x00000008
#define BASE_WITH_CTRL		0x00000010

#define WIN_MODE_NORMAL		0
#define WIN_MODE_RELOCATE	1
#define WIN_MODE_RESIZE		2
#define WIN_MODE_RELEASE	3

#define SLIDER_VAL_MAX		4096

typedef struct {
	short	xt, yt, xb, yb;
} RECT;

typedef struct {
	RECT	rect;		/* パーツの基本サイズ */
	void	*image;		/* 各パーツが使うイメージ・アレイ */
	short	type;		/* パーツのタイプ */
	int	flag;  		/* フラグ */
	int	action;		/* アクション・フラグ */
	void	*ext;		/* 各パーツが利用する拡張構造体のポインター */
	void 	(*run)(void);	/* 各パーツの条件で実行される関数 */
	void	*next;		/* 次のパーツ構造体のポインター */
} PART;

typedef struct {
	short	x, y, b;
} MOUSE;

typedef struct {
	PART	*part;
	short	prio;
	RECT	org;
} WINDOW;

typedef struct {
	char	*top;
	char	*pos;
	int	sx, sy;
	int	px, py;
	int	ox, oy;
} SLIDER;

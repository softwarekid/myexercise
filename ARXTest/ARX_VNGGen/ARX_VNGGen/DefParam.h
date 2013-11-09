#pragma once

/*********************************************************
 ******************** 图形参数 **************************
 *********************************************************/
/*
 * 节点之间的最小间距,
 * 在dot中默认为0.25英寸
 * 本程序默认为0.5英寸->12.7毫米
 */
#define DEF_NODE_SEP 12.7

/*
 * 垂直层次之间的最小间距
 * 在dot中默认为0.5英寸
 * 在本程序默认为1英寸->25.4毫米
 */
#define DEF_RANK_SEP 25.4

/* 默认图幅宽/高比值 */
#define DEF_GRAPH_RATIO 3

/* 默认图幅宽度800毫米 */
#define DEF_GRAPH_WIDTH 600

/* 默认图幅高度600毫米 */
#define DEF_GRAPH_HEIGHT 800

/*
 * 是否使用默认的宽高设置
 * 即不设置ratio以及宽度
 */
#define DEF_GRAPH_USE_DEF_WH 1

/*********************************************************
 ******************** 节点参数 **************************
 *********************************************************/
/* 
 * 节点的宽度 
 * 在dot中默认为0.75英寸
 */
#define DEF_NODE_WIDTH 19.05

/*
 * 节点的高度
 * 在dot中默认为0.5英寸
 */
#define DEF_NODE_HEIGHT 12.7

/*
 * dot的字体高度默认为14pt
 * 换算成mm==>14/72*25.4 = 4.94
 */
#define DEF_NODE_TEXT_HEIGHT 4.94

/*********************************************************
 ******************** 分支参数 **************************
 *********************************************************/
/*
 * 在dot中, 箭头宽度默认为1英寸
 * 在本程序中默认为4毫米
 */
#define DEF_ARROW_WIDTH 4

/* 
 * 箭头默认长度
 * 在本程序中默认为9毫米
 */
#define DEF_ARROW_LENGTH 9

/*
 * dot的字体高度默认为14pt
 * 换算成mm==>14/72*25.4 = 4.94
 */
#define DEF_EDGE_TEXT_HEIGHT 4.94

/* 默认不显示分支编号 */
#define DEF_NEED_EDGE 1


/*********************************************************
 ******************** 风门参数 **************************
 *********************************************************/
#define DEF_GATE_RADIUS 6
#define DEF_GATE_HALF_LENGTH 8


/*********************************************************
 ******************** 风机参数 **************************
 *********************************************************/
#define DEF_FAN_RADIUS 6
#define DEF_FAN_DISTANCE 22
/******************************************************************
 ** \file   Emm_V5.c
 **
 ** \author Yiiry
 **
 ** \brief  本文件（Emm_V5.c/.h）修改自张大头闭环步进电机驱动，主要功能为发送控制/读取命令
 **
 ** \post   可配合笔者封装的 zdt_api.c/.h 使用实现电机返回数据包的异步接收和解析
 **
 ** \note   相较于原版，主要修改内容包括：
 **
 **         - 采用 Doxygen 风格格式化函数注释，并用将类型名更换为更简洁的别名（u8，u16，u32 等等）.
 **
 **         - 函数增加形参串口号来适配多串口控制，并用 serial_send_emm_v5_cmd() 统一命令发送接口（若需移植直接改该函数即可）.
 **
 **         推荐配合 zdt_api.c/.h 使用，具体使用方法请参考文件中的函数注释和示例.
 */

#include "Emm_V5.h"

/**********************************************************
*** Emm_V5.0步进闭环控制例程
*** 编写作者：ZHANGDATOU
*** 技术支持：张大头闭环伺服
*** 淘宝店铺：https://zhangdatou.taobao.com
*** CSDN博客：https://blog.csdn.net/zhangdatou666
*** qq交流群：262438510
**********************************************************/

/* Global Variables -------------------------------------------------------- */

__IO u16 MMCL_count = 0, MMCL_cmd[ZDT_MMCL_LEN] = {0};

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief      发送 Emm_V5.0 命令
 * \param[in]  idx    串口索引
 * \param[in]  cmd    命令字符串
 *
 * \note       - 一开始想用 serial_send_string 函数直接发送字符串，所以在每个字符串末尾补上了 \0
 *               但是忘记了 \0 其实就是 0x00，会被串口发送函数当作字符串结束标志，导致发送不完整
 *               目前暂时用用 0x6B 替代 \0 作为字符串结束标志
 * \warning    严禁在此函数内/紧邻后部调用其他串口，否则会导致系统死机、串口端口混淆死机等一系列问题！！！
 */
void serial_send_emm_v5_cmd(srl_e idx, u8 *cmd)
{
    // 这里一定不能加其他串口

    while (zdtTvFlg); // ******** 可选行，需配合 zdt_api.c/.h 使用，等待清零 ********

    zdtTvFlg = true; // ******** 可选行，需配合 zdt_api.c/.h 使用，设置发送标志位 ********

    zdtTvTag[0] = cmd[0]; // ******** 可选行，需配合 zdt_api.c/.h 使用，记录地址信息 ********
    zdtTvTag[1] = cmd[1]; // ******** 可选行，需配合 zdt_api.c/.h 使用，记录功能码信息 ********

    u8 i;
    for (i = 0; cmd[i - 1] != 0x6B; i++) {
        serial_send_byte(idx, cmd[i]);
        // serial_send_byte(1, cmd[i]);
    }

    zdtTimeoutCnt = 0;    // ******** 可选行，需配合 zdt_api.c/.h 使用，重置超时计数 ********
    zdtTimeoutFlg = true; // ******** 可选行，需配合 zdt_api.c/.h 使用，设置超时标志位，开始计时 ********
}

/*





*/

/**********************************************************
*** 触发动作命令
**********************************************************/

/* 触发动作命令 ******************** */

/******************************************************************
 * \brief      触发编码器校准
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Trig_Encoder_Cal(srl_e idx, u8 addr)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x06; // 功能码
    cmd[2] = 0x45; // 辅助码
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      重启电机（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Reset_Motor(srl_e idx, u8 addr)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x08; // 功能码
    cmd[2] = 0x97; // 辅助码
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      将当前位置清零
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Reset_CurPos_To_Zero(srl_e idx, u8 addr)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x0A; // 功能码
    cmd[2] = 0x6D; // 辅助码
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      解除堵转保护
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Reset_Clog_Pro(srl_e idx, u8 addr)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x0E; // 功能码
    cmd[2] = 0x52; // 辅助码
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      恢复出厂设置
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Restore_Motor(srl_e idx, u8 addr)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x0F; // 功能码
    cmd[2] = 0x5F; // 辅助码
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/* ******************** 触发动作命令 */

/*





*/

/**********************************************************
*** 运动控制命令
**********************************************************/

/* 运动控制命令 ******************** */

/******************************************************************
 * \brief      多电机命令（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Multi_Motor_Cmd(srl_e idx, u8 addr)
{
    u8 i                            = 0;
    static u8 cmd[ZDT_MMCL_LEN + 4] = {0};

    cmd[i] = addr;
    ++i; // 地址
    cmd[i] = 0xEF;
    ++i; // 功能码
    cmd[i] = 0xCF;
    ++i; // 辅助码

    for (u8 j = 0; j < MMCL_count; ++j) {
        cmd[i] = MMCL_cmd[j];
        ++i; // 逐字节加载多机指令内容
    }

    cmd[i] = 0x6B;
    ++i; // 校验字节
    cmd[i] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);

    MMCL_count = 0; // 发送后清空缓冲计数
}

/******************************************************************
 * \brief      电机使能控制
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  state  使能状态，true 为使能电机，false 为关闭电机
 * \param[in]  snF    多机同步标志，false 为不启用，true 为启用
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_En_Control(srl_e idx, u8 addr, bool state, bool snF)
{
    static u8 cmd[7] = {0};

    cmd[0] = addr;      // 地址
    cmd[1] = 0xF3;      // 功能码
    cmd[2] = 0xAB;      // 辅助码
    cmd[3] = (u8)state; // 使能状态
    cmd[4] = snF;       // 多机同步运动标志
    cmd[5] = 0x6B;      // 校验字节
    cmd[6] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      速度模式控制
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  dir    方向，0 为 CW，其余值为 CCW
 * \param[in]  vel    速度，范围 0-5000 RPM
 * \param[in]  acc    加速度，范围 0-255，0 为直接启动
 * \param[in]  snF    多机同步标志，false 为不启用，true 为启用
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Vel_Control(srl_e idx, u8 addr, u8 dir, u16 vel, u8 acc, bool snF)
{
    static u8 cmd[9] = {0};

    cmd[0] = addr;           // 地址
    cmd[1] = 0xF6;           // 功能码
    cmd[2] = dir;            // 方向
    cmd[3] = (u8)(vel >> 8); // 速度高字节
    cmd[4] = (u8)(vel >> 0); // 速度低字节
    cmd[5] = acc;            // 加速度，0 为直接启动
    cmd[6] = snF;            // 多机同步标志
    cmd[7] = 0x6B;           // 校验字节
    cmd[8] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      位置模式控制
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  dir    方向，0 为 CW，其余值为 CCW
 * \param[in]  vel    速度，范围 0-5000 RPM
 * \param[in]  acc    加速度，范围 0-255，0 为直接启动
 * \param[in]  clk    脉冲数，范围 0-(2^32-1)
 * \param[in]  raF    相位/绝对标志，false 为相对，true 为绝对
 * \param[in]  snF    多机同步标志，false 为不启用，true 为启用
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 *
 * \note       每个脉冲转 1.8° / 16 = 0.1125°，
 *             发送 3200 个脉冲电机转一圈 360°；
 */
void Emm_V5_Pos_Control(srl_e idx, u8 addr, u8 dir, u16 vel, u8 acc, u32 clk, bool raF, bool snF)
{
    static u8 cmd[14] = {0};

    cmd[0]  = addr;            // 地址
    cmd[1]  = 0xFD;            // 功能码
    cmd[2]  = dir;             // 方向
    cmd[3]  = (u8)(vel >> 8);  // 速度高字节
    cmd[4]  = (u8)(vel >> 0);  // 速度低字节
    cmd[5]  = acc;             // 加速度
    cmd[6]  = (u8)(clk >> 24); // 脉冲数 bit24-31
    cmd[7]  = (u8)(clk >> 16); // 脉冲数 bit16-23
    cmd[8]  = (u8)(clk >> 8);  // 脉冲数 bit8-15
    cmd[9]  = (u8)(clk >> 0);  // 脉冲数 bit0-7
    cmd[10] = raF;             // 相对/绝对标志
    cmd[11] = snF;             // 多机同步标志
    cmd[12] = 0x6B;            // 校验字节
    cmd[13] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      立即停止
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  snF    多机同步标志，false 为不启用，true 为启用
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Stop_Now(srl_e idx, u8 addr, bool snF)
{
    static u8 cmd[6] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0xFE; // 功能码
    cmd[2] = 0x98; // 辅助码
    cmd[3] = snF;  // 多机同步标志
    cmd[4] = 0x6B; // 校验字节
    cmd[5] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      多机同步运动
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Synchronous_motion(srl_e idx, u8 addr)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0xFF; // 功能码
    cmd[2] = 0x66; // 辅助码
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/* ******************** 运动控制命令 */

/*





*/

/**********************************************************
*** 原点回零命令
**********************************************************/

/* 原点回零命令 ******************** */

/******************************************************************
 * \brief      设置单圈回零的零点位置
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Origin_Set_O(srl_e idx, u8 addr, bool svF)
{
    static u8 cmd[6] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x93; // 功能码
    cmd[2] = 0x88; // 辅助码
    cmd[3] = svF;  // 是否存储标志
    cmd[4] = 0x6B; // 校验字节
    cmd[5] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      触发回零
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  o_mode 回零模式
 * \param[in]  snF    多机同步标志，false 为不启用，true 为启用
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Origin_Trigger_Return(srl_e idx, u8 addr, u8 o_mode, bool snF)
{
    static u8 cmd[6] = {0};

    cmd[0] = addr;   // 地址
    cmd[1] = 0x9A;   // 功能码
    cmd[2] = o_mode; // 回零模式
    cmd[3] = snF;    // 多机同步标志
    cmd[4] = 0x6B;   // 校验字节
    cmd[5] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      强制中断并退出回零
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Origin_Interrupt(srl_e idx, u8 addr)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x9C; // 功能码
    cmd[2] = 0x48; // 辅助码
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      读取回零参数
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Origin_Read_Params(srl_e idx, u8 addr)
{
    static u8 cmd[4] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x22; // 功能码
    cmd[2] = 0x6B; // 校验字节
    cmd[3] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改回零参数
 * \param[in]  idx  串口号
 * \param[in]  addr    电机地址
 * \param[in]  svF     是否存储标志，false 为不存储，true 为存储
 * \param[in]  o_mode  回零模式
 * \param[in]  o_dir   回零方向，0 为 CW，其余值为 CCW
 * \param[in]  o_vel   回零速度，单位 RPM
 * \param[in]  o_tm    回零超时时间，单位 ms
 * \param[in]  sl_vel  无限位碰撞回零检测转速，单位 RPM
 * \param[in]  sl_ma   无限位碰撞回零检测电流，单位 mA
 * \param[in]  sl_ms   无限位碰撞回零检测时间，单位 ms
 * \param[in]  potF    上电自动触发回零标志
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Origin_Modify_Params(srl_e idx, u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, u32 o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF)
{
    static u8 cmd[21] = {0};

    cmd[0]  = addr;              // 地址
    cmd[1]  = 0x4C;              // 功能码
    cmd[2]  = 0xAE;              // 辅助码
    cmd[3]  = svF;               // 是否存储标志
    cmd[4]  = o_mode;            // 回零模式
    cmd[5]  = o_dir;             // 回零方向
    cmd[6]  = (u8)(o_vel >> 8);  // 回零速度高字节
    cmd[7]  = (u8)(o_vel >> 0);  // 回零速度低字节
    cmd[8]  = (u8)(o_tm >> 24);  // 回零超时 bit24-31
    cmd[9]  = (u8)(o_tm >> 16);  // 回零超时 bit16-23
    cmd[10] = (u8)(o_tm >> 8);   // 回零超时 bit8-15
    cmd[11] = (u8)(o_tm >> 0);   // 回零超时 bit0-7
    cmd[12] = (u8)(sl_vel >> 8); // 碰撞检测转速高字节
    cmd[13] = (u8)(sl_vel >> 0); // 碰撞检测转速低字节
    cmd[14] = (u8)(sl_ma >> 8);  // 碰撞检测电流高字节
    cmd[15] = (u8)(sl_ma >> 0);  // 碰撞检测电流低字节
    cmd[16] = (u8)(sl_ms >> 8);  // 碰撞检测时间高字节
    cmd[17] = (u8)(sl_ms >> 0);  // 碰撞检测时间低字节
    cmd[18] = potF;              // 上电自动回零标志
    cmd[19] = 0x6B;              // 校验字节
    cmd[20] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/* ********************** 原点回零命令 */

/*





*/

/**********************************************************
*** 读取系统参数命令
**********************************************************/

/* 读取系统参数命令 ******************** */

/******************************************************************
 * \brief      定时返回信息命令（Y42）
 * \param[in]  idx  串口号
 * \param[in]  addr    电机地址
 * \param[in]  s       系统参数类型
 * \param[in]  time_ms 定时时间
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Auto_Return_Sys_Params_Timed(srl_e idx, u8 addr, zdtSysParams_t s, u16 time_ms)
{
    u8 i              = 0;
    static u8 cmd[16] = {0};

    cmd[i++] = addr; // 地址
    cmd[i++] = 0x11; // 功能码
    cmd[i++] = 0x18; // 辅助码

    switch (s) {
        case S_VBUS:
            cmd[i++] = 0x24; // 读取总线电压
            break;
        case S_CBUS:
            cmd[i++] = 0x26; // 读取总线电流
            break;
        case S_CPHA:
            cmd[i++] = 0x27; // 读取相电流
            break;
        case S_ENCO:
            cmd[i++] = 0x29; // 读取编码器原始值
            break;
        case S_CLKC:
            cmd[i++] = 0x30; // 读取实时脉冲数
            break;
        case S_ENCL:
            cmd[i++] = 0x31; // 读取线性化后编码器值
            break;
        case S_CLKI:
            cmd[i++] = 0x32; // 读取输入脉冲数
            break;
        case S_TPOS:
            cmd[i++] = 0x33; // 读取目标位置
            break;
        case S_SPOS:
            cmd[i++] = 0x34; // 读取实时设定目标位置
            break;
        case S_VEL:
            cmd[i++] = 0x35; // 读取实时转速
            break;
        case S_CPOS:
            cmd[i++] = 0x36; // 读取实时位置
            break;
        case S_PERR:
            cmd[i++] = 0x37; // 读取位置误差
            break;
        case S_VBAT:
            cmd[i++] = 0x38; // 读取多圈编码器电池电压（Y42）
            break;
        case S_TEMP:
            cmd[i++] = 0x39; // 读取实时温度（Y42）
            break;
        case S_FLAG:
            cmd[i++] = 0x3A; // 读取状态标志位
            break;
        case S_OFLAG:
            cmd[i++] = 0x3B; // 读取回零状态标志位
            break;
        case S_OAF:
            cmd[i++] = 0x3C; // 读取状态+回零标志位（Y42）
            break;
        case S_PIN:
            cmd[i++] = 0x3D; // 读取引脚状态（Y42）
            break;
        default:
            break;
    }

    cmd[i++] = (u8)(time_ms >> 8); // 定时时间高字节
    cmd[i++] = (u8)(time_ms >> 0); // 定时时间低字节
    cmd[i++] = 0x6B;               // 校验字节
    cmd[i]   = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      读取系统参数
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  s      系统参数类型
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_Sys_Params(srl_e idx, u8 addr, zdtSysParams_t s)
{
    u8 i              = 0;
    static u8 cmd[16] = {0};

    cmd[i++] = addr; // 地址

    switch (s) {
        case S_VBUS:
            cmd[i++] = 0x24; // 读取总线电压
            break;
        case S_CBUS:
            cmd[i++] = 0x26; // 读取总线电流
            break;
        case S_CPHA:
            cmd[i++] = 0x27; // 读取相电流
            break;
        case S_ENCO:
            cmd[i++] = 0x29; // 读取编码器原始值
            break;
        case S_CLKC:
            cmd[i++] = 0x30; // 读取实时脉冲数
            break;
        case S_ENCL:
            cmd[i++] = 0x31; // 读取线性化后编码器值
            break;
        case S_CLKI:
            cmd[i++] = 0x32; // 读取输入脉冲数
            break;
        case S_TPOS:
            cmd[i++] = 0x33; // 读取目标位置
            break;
        case S_SPOS:
            cmd[i++] = 0x34; // 读取实时设定目标位置
            break;
        case S_VEL:
            cmd[i++] = 0x35; // 读取实时转速
            break;
        case S_CPOS:
            cmd[i++] = 0x36; // 读取实时位置
            break;
        case S_PERR:
            cmd[i++] = 0x37; // 读取位置误差
            break;
        case S_VBAT:
            cmd[i++] = 0x38; // 读取多圈编码器电池电压（Y42）
            break;
        case S_TEMP:
            cmd[i++] = 0x39; // 读取实时温度（Y42）
            break;
        case S_FLAG:
            cmd[i++] = 0x3A; // 读取状态标志位
            break;
        case S_OFLAG:
            cmd[i++] = 0x3B; // 读取回零状态标志位
            break;
        case S_OAF:
            cmd[i++] = 0x3C; // 读取状态+回零标志位（Y42）
            break;
        case S_PIN:
            cmd[i++] = 0x3D; // 读取引脚状态（Y42）
            break;
        default:
            break;
    }

    cmd[i++] = 0x6B; // 校验字节
    cmd[i]   = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/* ******************** 读取系统参数命令 */

/*





*/

/**********************************************************
*** 读写驱动参数命令
**********************************************************/

/* 读写驱动参数命令 ******************** */

/******************************************************************
 * \brief      修改电机 ID 地址
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  id     默认电机 ID 为 1，可修改为 1-255，0 为广播地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Motor_ID(srl_e idx, u8 addr, bool svF, u8 id)
{
    static u8 cmd[7] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0xAE; // 功能码
    cmd[2] = 0x4B; // 辅助码
    cmd[3] = svF;  // 是否存储标志
    cmd[4] = id;   // 新 ID
    cmd[5] = 0x6B; // 校验字节
    cmd[6] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改细分值
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  mstep  默认细分为 16，可修改为 1-255，0 为 256 细分
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_MicroStep(srl_e idx, u8 addr, bool svF, u8 mstep)
{
    static u8 cmd[7] = {0};

    cmd[0] = addr;  // 地址
    cmd[1] = 0x84;  // 功能码
    cmd[2] = 0x8A;  // 辅助码
    cmd[3] = svF;   // 是否存储标志
    cmd[4] = mstep; // 细分值
    cmd[5] = 0x6B;  // 校验字节
    cmd[6] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改掉电标志
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  pdf    掉电标志
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_PDFlag(srl_e idx, u8 addr, bool pdf)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x50; // 功能码
    cmd[2] = pdf;  // 掉电标志
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      读取选项参数状态（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_Opt_Param_Sta(srl_e idx, u8 addr)
{
    static u8 cmd[4] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x1A; // 功能码
    cmd[2] = 0x6B; // 校验字节
    cmd[3] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改电机类型（Y42）
 * \param[in]  idx  串口号
 * \param[in]  addr    电机地址
 * \param[in]  svF     是否存储标志，false 为不存储，true 为存储
 * \param[in]  mottype 电机类型，0 为 1.8°，1 为 0.9°
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Motor_Type(srl_e idx, u8 addr, bool svF, bool mottype)
{
    static u8 cmd[7] = {0};
    u8 MotType       = mottype ? 25 : 50; // 25 对应 0.9°，50 对应 1.8°

    cmd[0] = addr;    // 地址
    cmd[1] = 0xD7;    // 功能码
    cmd[2] = 0x35;    // 辅助码
    cmd[3] = svF;     // 是否存储标志
    cmd[4] = MotType; // 电机类型
    cmd[5] = 0x6B;    // 校验字节
    cmd[6] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改固件类型（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  fwtype 固件类型，0 为 X 固件，1 为 Emm 固件
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Firmware_Type(srl_e idx, u8 addr, bool svF, bool fwtype)
{
    static u8 cmd[7] = {0};

    cmd[0] = addr;   // 地址
    cmd[1] = 0xD5;   // 功能码
    cmd[2] = 0x69;   // 辅助码
    cmd[3] = svF;    // 是否存储标志
    cmd[4] = fwtype; // 固件类型
    cmd[5] = 0x6B;   // 校验字节
    cmd[6] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改开环/闭环控制模式（Y42）
 * \param[in]  idx    串口号
 * \param[in]  addr      电机地址
 * \param[in]  svF       是否存储标志，false 为不存储，true 为存储
 * \param[in]  ctrl_mode 控制模式，0 为开环，1 为闭环 FOC
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Ctrl_Mode(srl_e idx, u8 addr, bool svF, bool ctrl_mode)
{
    static u8 cmd[7] = {0};

    cmd[0] = addr;      // 地址
    cmd[1] = 0x46;      // 功能码
    cmd[2] = 0x69;      // 辅助码
    cmd[3] = svF;       // 是否存储标志
    cmd[4] = ctrl_mode; // 控制模式
    cmd[5] = 0x6B;      // 校验字节
    cmd[6] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改电机运动正方向（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  dir    电机运动正方向，0 为 CW，1 为 CCW
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Motor_Dir(srl_e idx, u8 addr, bool svF, bool dir)
{
    static u8 cmd[7] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0xD4; // 功能码
    cmd[2] = 0x60; // 辅助码
    cmd[3] = svF;  // 是否存储标志
    cmd[4] = dir;  // 方向
    cmd[5] = 0x6B; // 校验字节
    cmd[6] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改锁定按键功能（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  lock   锁定按键功能，0 为 Disable，1 为 Enable
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Lock_Btn(srl_e idx, u8 addr, bool svF, bool lock)
{
    static u8 cmd[7] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0xD0; // 功能码
    cmd[2] = 0xB3; // 辅助码
    cmd[3] = svF;  // 是否存储标志
    cmd[4] = lock; // 锁定按键功能
    cmd[5] = 0x6B; // 校验字节
    cmd[6] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改命令速度值是否缩小 10 倍输入（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  s_vel  命令速度值是否缩小 10 倍输入
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_S_Vel(srl_e idx, u8 addr, bool svF, bool s_vel)
{
    static u8 cmd[7] = {0};

    cmd[0] = addr;  // 地址
    cmd[1] = 0x4F;  // 功能码
    cmd[2] = 0x71;  // 辅助码
    cmd[3] = svF;   // 是否存储标志
    cmd[4] = s_vel; // 缩小 10 倍标志
    cmd[5] = 0x6B;  // 校验字节
    cmd[6] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改开环模式工作电流
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  om_ma  开环模式工作电流，单位 mA
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_OM_mA(srl_e idx, u8 addr, bool svF, u16 om_ma)
{
    static u8 cmd[8] = {0};

    cmd[0] = addr;             // 地址
    cmd[1] = 0x44;             // 功能码
    cmd[2] = 0x33;             // 辅助码
    cmd[3] = svF;              // 是否存储标志
    cmd[4] = (u8)(om_ma >> 8); // 工作电流高字节
    cmd[5] = (u8)(om_ma >> 0); // 工作电流低字节
    cmd[6] = 0x6B;             // 校验字节
    cmd[7] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改闭环模式最大电流
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  foc_mA 闭环模式最大电流，单位 mA
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_FOC_mA(srl_e idx, u8 addr, bool svF, u16 foc_mA)
{
    static u8 cmd[8] = {0};

    cmd[0] = addr;              // 地址
    cmd[1] = 0x45;              // 功能码
    cmd[2] = 0x66;              // 辅助码
    cmd[3] = svF;               // 是否存储标志
    cmd[4] = (u8)(foc_mA >> 8); // 最大电流高字节
    cmd[5] = (u8)(foc_mA >> 0); // 最大电流低字节
    cmd[6] = 0x6B;              // 校验字节
    cmd[7] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      读取 PID 参数
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_PID_Params(srl_e idx, u8 addr)
{
    static u8 cmd[4] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x21; // 功能码
    cmd[2] = 0x6B; // 校验字节
    cmd[3] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改 PID 参数
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  kp     比例系数
 * \param[in]  ki     积分系数
 * \param[in]  kd     微分系数
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_PID_Params(srl_e idx, u8 addr, bool svF, u32 kp, u32 ki, u32 kd)
{
    static u8 cmd[18] = {0};

    cmd[0]  = addr;           // 地址
    cmd[1]  = 0x4A;           // 功能码
    cmd[2]  = 0xC3;           // 辅助码
    cmd[3]  = svF;            // 是否存储标志
    cmd[4]  = (u8)(kp >> 24); // kp 高字节
    cmd[5]  = (u8)(kp >> 16);
    cmd[6]  = (u8)(kp >> 8);
    cmd[7]  = (u8)(kp >> 0);
    cmd[8]  = (u8)(ki >> 24); // ki 高字节
    cmd[9]  = (u8)(ki >> 16);
    cmd[10] = (u8)(ki >> 8);
    cmd[11] = (u8)(ki >> 0);
    cmd[12] = (u8)(kd >> 24); // kd 高字节
    cmd[13] = (u8)(kd >> 16);
    cmd[14] = (u8)(kd >> 8);
    cmd[15] = (u8)(kd >> 0);
    cmd[16] = 0x6B; // 校验字节
    cmd[17] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      读取 DMX512 协议参数（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_DMX512_Params(srl_e idx, u8 addr)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x49; // 功能码
    cmd[2] = 0x78; // 辅助码
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改 DMX512 协议参数（Y42）
 * \param[in]  idx   串口号
 * \param[in]  addr     电机地址
 * \param[in]  svF      是否存储标志，false 为不存储，true 为存储
 * \param[in]  tch      总通道数
 * \param[in]  nch      每个电机占用的通道数
 * \param[in]  mode     运动模式，0 为相对位置模式，1 为绝对坐标式
 * \param[in]  vel      单通道模式速度，单位 RPM
 * \param[in]  acc      加速度
 * \param[in]  vel_step 双通道模式速度步长
 * \param[in]  pos_step 双通道模式运动步长
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_DMX512_Params(srl_e idx, u8 addr, bool svF, u16 tch, u8 nch, u8 mode, u16 vel, u16 acc, u16 vel_step, u32 pos_step)
{
    static u8 cmd[20] = {0};

    cmd[0]  = addr;                 // 地址
    cmd[1]  = 0xD9;                 // 功能码
    cmd[2]  = 0x90;                 // 辅助码
    cmd[3]  = svF;                  // 是否存储标志
    cmd[4]  = (u8)(tch >> 8);       // 总通道数高字节
    cmd[5]  = (u8)(tch >> 0);       // 总通道数低字节
    cmd[6]  = nch;                  // 每个电机占用的通道数
    cmd[7]  = mode;                 // 运动模式
    cmd[8]  = (u8)(vel >> 8);       // 单通道速度高字节
    cmd[9]  = (u8)(vel >> 0);       // 单通道速度低字节
    cmd[10] = (u8)(acc >> 8);       // 加速度高字节
    cmd[11] = (u8)(acc >> 0);       // 加速度低字节
    cmd[12] = (u8)(vel_step >> 8);  // 双通道速度步长高字节
    cmd[13] = (u8)(vel_step >> 0);  // 双通道速度步长低字节
    cmd[14] = (u8)(pos_step >> 24); // 运动步长 bit24-31
    cmd[15] = (u8)(pos_step >> 16); // 运动步长 bit16-23
    cmd[16] = (u8)(pos_step >> 8);  // 运动步长 bit8-15
    cmd[17] = (u8)(pos_step >> 0);  // 运动步长 bit0-7
    cmd[18] = 0x6B;                 // 校验字节
    cmd[19] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      读取位置到达窗口（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_Pos_Window(srl_e idx, u8 addr)
{
    static u8 cmd[4] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x41; // 功能码
    cmd[2] = 0x6B; // 校验字节
    cmd[3] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改位置到达窗口（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  prw    位置到达窗口，默认 8（0.8°）
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Pos_Window(srl_e idx, u8 addr, bool svF, u16 prw)
{
    static u8 cmd[8] = {0};

    cmd[0] = addr;           // 地址
    cmd[1] = 0xD1;           // 功能码
    cmd[2] = 0x07;           // 辅助码
    cmd[3] = svF;            // 是否存储标志
    cmd[4] = (u8)(prw >> 8); // 窗口高字节
    cmd[5] = (u8)(prw >> 0); // 窗口低字节
    cmd[6] = 0x6B;           // 校验字节
    cmd[7] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      读取过热过流保护检测阈值（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_Otocp(srl_e idx, u8 addr)
{
    static u8 cmd[4] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x13; // 功能码
    cmd[2] = 0x6B; // 校验字节
    cmd[3] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改过热过流保护检测阈值（Y42）
 * \param[in]  idx  串口号
 * \param[in]  addr    电机地址
 * \param[in]  svF     是否存储标志，false 为不存储，true 为存储
 * \param[in]  otp     过热保护检测阈值
 * \param[in]  ocp     过流保护检测阈值
 * \param[in]  time_ms 过热过流检测时间
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Otocp(srl_e idx, u8 addr, bool svF, u16 otp, u16 ocp, u16 time_ms)
{
    static u8 cmd[12] = {0};

    cmd[0]  = addr;               // 地址
    cmd[1]  = 0xD3;               // 功能码
    cmd[2]  = 0x56;               // 辅助码
    cmd[3]  = svF;                // 是否存储标志
    cmd[4]  = (u8)(otp >> 8);     // 过热阈值高字节
    cmd[5]  = (u8)(otp >> 0);     // 过热阈值低字节
    cmd[6]  = (u8)(ocp >> 8);     // 过流阈值高字节
    cmd[7]  = (u8)(ocp >> 0);     // 过流阈值低字节
    cmd[8]  = (u8)(time_ms >> 8); // 检测时间高字节
    cmd[9]  = (u8)(time_ms >> 0); // 检测时间低字节
    cmd[10] = 0x6B;               // 校验字节
    cmd[11] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      读取心跳保护功能时间（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_Heart_Protect(srl_e idx, u8 addr)
{
    static u8 cmd[4] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x16; // 功能码
    cmd[2] = 0x6B; // 校验字节
    cmd[3] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改心跳保护功能时间（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  hp     心跳保护时间，单位 ms
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Heart_Protect(srl_e idx, u8 addr, bool svF, u32 hp)
{
    static u8 cmd[10] = {0};

    cmd[0] = addr;           // 地址
    cmd[1] = 0x68;           // 功能码
    cmd[2] = 0x38;           // 辅助码
    cmd[3] = svF;            // 是否存储标志
    cmd[4] = (u8)(hp >> 24); // 时间 bit24-31
    cmd[5] = (u8)(hp >> 16); // 时间 bit16-23
    cmd[6] = (u8)(hp >> 8);  // 时间 bit8-15
    cmd[7] = (u8)(hp >> 0);  // 时间 bit0-7
    cmd[8] = 0x6B;           // 校验字节
    cmd[9] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      读取积分限幅/刚性系数（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_Integral_Limit(srl_e idx, u8 addr)
{
    static u8 cmd[4] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x23; // 功能码
    cmd[2] = 0x6B; // 校验字节
    cmd[3] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      修改积分限幅/刚性系数（Y42）
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志，false 为不存储，true 为存储
 * \param[in]  il     积分限幅
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Integral_Limit(srl_e idx, u8 addr, bool svF, u32 il)
{
    static u8 cmd[10] = {0};

    cmd[0] = addr;           // 地址
    cmd[1] = 0x4B;           // 功能码
    cmd[2] = 0x57;           // 辅助码
    cmd[3] = svF;            // 是否存储标志
    cmd[4] = (u8)(il >> 24); // 积分限幅 bit24-31
    cmd[5] = (u8)(il >> 16); // 积分限幅 bit16-23
    cmd[6] = (u8)(il >> 8);  // 积分限幅 bit8-15
    cmd[7] = (u8)(il >> 0);  // 积分限幅 bit0-7
    cmd[8] = 0x6B;           // 校验字节
    cmd[9] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/* ******************** 读写驱动参数命令 */

/*





*/

/**********************************************************
*** 读取所有驱动参数命令
**********************************************************/

/* 读取所有驱动参数命令 ******************** */

/******************************************************************
 * \brief      读取系统状态参数
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_System_State_Params(srl_e idx, u8 addr)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x43; // 功能码
    cmd[2] = 0x7A; // 辅助码
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/******************************************************************
 * \brief      读取驱动配置参数
 * \param[in]  idx    串口索引
 * \param[in]  addr   电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_Motor_Conf_Params(srl_e idx, u8 addr)
{
    static u8 cmd[5] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x42; // 功能码
    cmd[2] = 0x6C; // 辅助码
    cmd[3] = 0x6B; // 校验字节
    cmd[4] = '\0';

    serial_send_emm_v5_cmd(idx, cmd);
}

/* ******************** 读写驱动参数命令 */

/*





*/

/**********************************************************
*** 以下是把相应命令加载到 Y42 多电机命令上的函数（Y42）
**********************************************************/

/*





*/

/**********************************************************
*** 触发动作命令
**********************************************************/

/* 触发动作命令 ******************** */

/******************************************************************
 * \brief      触发编码器校准 - 加载到多电机指令上
 * \param[in]  addr  电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Trig_Encoder_Cal(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x06; // 功能码
    cmd[2] = 0x45; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      重启电机（Y42） - 加载到多电机指令上
 * \param[in]  addr  电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Reset_Motor(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x08; // 功能码
    cmd[2] = 0x97; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      将当前位置清零 - 加载到多电机指令上
 * \param[in]  addr  电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Reset_CurPos_To_Zero(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x0A; // 功能码
    cmd[2] = 0x6D; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      解除堵转保护 - 加载到多电机指令上
 * \param[in]  addr  电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Reset_Clog_Pro(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x0E; // 功能码
    cmd[2] = 0x52; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      恢复出厂设置 - 加载到多电机指令上
 * \param[in]  addr  电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Restore_Motor(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x0F; // 功能码
    cmd[2] = 0x5F; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/* ******************** 触发动作命令 */

/*





*/

/**********************************************************
*** 运动控制命令（多机）
**********************************************************/

/* 运动控制命令（多机） ******************** */

/******************************************************************
 * \brief      使能信号控制 - 加载到多电机指令上
 * \param[in]  addr  电机地址
 * \param[in]  state 使能状态，true 为使能，false 为关闭
 * \param[in]  snF   多机同步标志，false 为不启用，true 为启用
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_En_Control(u8 addr, bool state, bool snF)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr;      // 地址
    cmd[1] = 0xF3;      // 功能码
    cmd[2] = 0xAB;      // 辅助码
    cmd[3] = (u8)state; // 使能状态
    cmd[4] = snF;       // 多机同步标志
    cmd[5] = 0x6B;      // 校验字节

    for (j = 0; j < 6; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      速度模式 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  dir  方向，0 为 CW，其余为 CCW
 * \param[in]  vel  速度 0-5000RPM
 * \param[in]  acc  加速度 0-255，0 为直接启动
 * \param[in]  snF  多机同步标志，false 为不启用，true 为启用
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Vel_Control(u8 addr, u8 dir, u16 vel, u8 acc, bool snF)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr;           // 地址
    cmd[1] = 0xF6;           // 功能码
    cmd[2] = dir;            // 方向
    cmd[3] = (u8)(vel >> 8); // 速度高字节
    cmd[4] = (u8)(vel >> 0); // 速度低字节
    cmd[5] = acc;            // 加速度
    cmd[6] = snF;            // 多机同步标志
    cmd[7] = 0x6B;           // 校验字节

    for (j = 0; j < 8; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      位置模式 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  dir  方向，0 为 CW，其余为 CCW
 * \param[in]  vel  速度 0-5000RPM
 * \param[in]  acc  加速度 0-255，0 为直接启动
 * \param[in]  clk  脉冲数 0-(2^32-1)
 * \param[in]  raF  相对/绝对标志，false 为相对，true 为绝对
 * \param[in]  snF  多机同步标志，false 为不启用，true 为启用
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Pos_Control(u8 addr, u8 dir, u16 vel, u8 acc, u32 clk, bool raF, bool snF)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0]  = addr;            // 地址
    cmd[1]  = 0xFD;            // 功能码
    cmd[2]  = dir;             // 方向
    cmd[3]  = (u8)(vel >> 8);  // 速度高字节
    cmd[4]  = (u8)(vel >> 0);  // 速度低字节
    cmd[5]  = acc;             // 加速度
    cmd[6]  = (u8)(clk >> 24); // 脉冲数 bit24-31
    cmd[7]  = (u8)(clk >> 16); // 脉冲数 bit16-23
    cmd[8]  = (u8)(clk >> 8);  // 脉冲数 bit8-15
    cmd[9]  = (u8)(clk >> 0);  // 脉冲数 bit0-7
    cmd[10] = raF;             // 相对/绝对标志
    cmd[11] = snF;             // 多机同步标志
    cmd[12] = 0x6B;            // 校验字节

    for (j = 0; j < 13; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      立即停止 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  snF  多机同步标志，false 为不启用，true 为启用
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Stop_Now(u8 addr, bool snF)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0xFE; // 功能码
    cmd[2] = 0x98; // 辅助码
    cmd[3] = snF;  // 多机同步标志
    cmd[4] = 0x6B; // 校验字节

    for (j = 0; j < 5; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      多机同步运动 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Synchronous_motion(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0xFF; // 功能码
    cmd[2] = 0x66; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/* ******************** 运动控制命令（多机） */

/*





*/

/**********************************************************
*** 原点回零命令（多机）
**********************************************************/

/* 原点回零命令（多机） ******************** */

/******************************************************************
 * \brief      设置单圈回零零点 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  svF  是否存储标志
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Origin_Set_O(u8 addr, bool svF)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x93; // 功能码
    cmd[2] = 0x88; // 辅助码
    cmd[3] = svF;  // 是否存储标志
    cmd[4] = 0x6B; // 校验字节

    for (j = 0; j < 5; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      触发回零 - 加载到多电机指令上
 * \param[in]  addr  电机地址
 * \param[in]  o_mode 回零模式
 * \param[in]  snF   多机同步标志
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Origin_Trigger_Return(u8 addr, u8 o_mode, bool snF)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr;   // 地址
    cmd[1] = 0x9A;   // 功能码
    cmd[2] = o_mode; // 回零模式
    cmd[3] = snF;    // 多机同步标志
    cmd[4] = 0x6B;   // 校验字节

    for (j = 0; j < 5; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      强制中断并退出回零 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Origin_Interrupt(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    cmd[0] = addr; // 地址
    cmd[1] = 0x9C; // 功能码
    cmd[2] = 0x48; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      修改回零参数 - 加载到多电机指令上
 * \param[in]  addr   电机地址
 * \param[in]  svF    是否存储标志
 * \param[in]  o_mode 回零模式
 * \param[in]  o_dir  回零方向
 * \param[in]  o_vel  回零速度
 * \param[in]  o_tm   回零超时时间
 * \param[in]  sl_vel 碰撞回零检测转速
 * \param[in]  sl_ma  碰撞回零检测电流
 * \param[in]  sl_ms  碰撞回零检测时间
 * \param[in]  potF   上电自动回零标志
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Origin_Modify_Params(u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, u32 o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF)
{
    u8 j = 0, cmd[32] = {0};

    cmd[0]  = addr;              // 地址
    cmd[1]  = 0x4C;              // 功能码
    cmd[2]  = 0xAE;              // 辅助码
    cmd[3]  = svF;               // 是否存储标志
    cmd[4]  = o_mode;            // 回零模式
    cmd[5]  = o_dir;             // 回零方向
    cmd[6]  = (u8)(o_vel >> 8);  // 回零速度高字节
    cmd[7]  = (u8)(o_vel >> 0);  // 回零速度低字节
    cmd[8]  = (u8)(o_tm >> 24);  // 回零超时 bit24-31
    cmd[9]  = (u8)(o_tm >> 16);  // 回零超时 bit16-23
    cmd[10] = (u8)(o_tm >> 8);   // 回零超时 bit8-15
    cmd[11] = (u8)(o_tm >> 0);   // 回零超时 bit0-7
    cmd[12] = (u8)(sl_vel >> 8); // 碰撞检测转速高字节
    cmd[13] = (u8)(sl_vel >> 0); // 碰撞检测转速低字节
    cmd[14] = (u8)(sl_ma >> 8);  // 碰撞检测电流高字节
    cmd[15] = (u8)(sl_ma >> 0);  // 碰撞检测电流低字节
    cmd[16] = (u8)(sl_ms >> 8);  // 碰撞检测时间高字节
    cmd[17] = (u8)(sl_ms >> 0);  // 碰撞检测时间低字节
    cmd[18] = potF;              // 上电自动回零标志
    cmd[19] = 0x6B;              // 校验字节

    for (j = 0; j < 20; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/* ******************** 原点回零命令（多机） */

/*





*/

/**********************************************************
*** 读取系统参数命令（多机）
**********************************************************/

/* 读取系统参数命令（多机） ******************** */

/******************************************************************
 * \brief      定时返回信息命令（Y42）- 加载到多电机指令上
 * \param[in]  addr    电机地址
 * \param[in]  s       系统参数类型
 * \param[in]  time_ms 定时时间
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Auto_Return_Sys_Params_Timed(u8 addr, zdtSysParams_t s, u16 time_ms)
{
    u8 i = 0, j = 0;
    u8 cmd[16] = {0};

    cmd[i++] = addr; // 地址
    cmd[i++] = 0x11; // 功能码
    cmd[i++] = 0x18; // 辅助码

    switch (s) {
        case S_VBUS:
            cmd[i++] = 0x24; // 读取总线电压
            break;
        case S_CBUS:
            cmd[i++] = 0x26; // 读取总线电流
            break;
        case S_CPHA:
            cmd[i++] = 0x27; // 读取相电流
            break;
        case S_ENCO:
            cmd[i++] = 0x29; // 读取编码器原始值
            break;
        case S_CLKC:
            cmd[i++] = 0x30; // 读取实时脉冲数
            break;
        case S_ENCL:
            cmd[i++] = 0x31; // 读取线性化后编码器值
            break;
        case S_CLKI:
            cmd[i++] = 0x32; // 读取输入脉冲数
            break;
        case S_TPOS:
            cmd[i++] = 0x33; // 读取目标位置
            break;
        case S_SPOS:
            cmd[i++] = 0x34; // 读取实时设定目标位置
            break;
        case S_VEL:
            cmd[i++] = 0x35; // 读取实时转速
            break;
        case S_CPOS:
            cmd[i++] = 0x36; // 读取实时位置
            break;
        case S_PERR:
            cmd[i++] = 0x37; // 读取位置误差
            break;
        case S_VBAT:
            cmd[i++] = 0x38; // 读取多圈编码器电池电压（Y42）
            break;
        case S_TEMP:
            cmd[i++] = 0x39; // 读取实时温度（Y42）
            break;
        case S_FLAG:
            cmd[i++] = 0x3A; // 读取状态标志位
            break;
        case S_OFLAG:
            cmd[i++] = 0x3B; // 读取回零状态标志位
            break;
        case S_OAF:
            cmd[i++] = 0x3C; // 读取状态+回零标志位（Y42）
            break;
        case S_PIN:
            cmd[i++] = 0x3D; // 读取引脚状态（Y42）
            break;
        default:
            break;
    }

    cmd[i++] = (u8)(time_ms >> 8); // 定时时间高字节
    cmd[i++] = (u8)(time_ms >> 0); // 定时时间低字节
    cmd[i++] = 0x6B;               // 校验字节

    for (j = 0; j < i; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      读取系统参数 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  s    系统参数类型
 * \return     地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_MMCL_Read_Sys_Params(u8 addr, zdtSysParams_t s)
{
    u8 i = 0, j = 0;
    u8 cmd[16] = {0};

    cmd[i++] = addr; // 地址

    switch (s) {
        case S_VBUS:
            cmd[i++] = 0x24; // 读取总线电压
            break;
        case S_CBUS:
            cmd[i++] = 0x26; // 读取总线电流
            break;
        case S_CPHA:
            cmd[i++] = 0x27; // 读取相电流
            break;
        case S_ENCO:
            cmd[i++] = 0x29; // 读取编码器原始值
            break;
        case S_CLKC:
            cmd[i++] = 0x30; // 读取实时脉冲数
            break;
        case S_ENCL:
            cmd[i++] = 0x31; // 读取线性化后编码器值
            break;
        case S_CLKI:
            cmd[i++] = 0x32; // 读取输入脉冲数
            break;
        case S_TPOS:
            cmd[i++] = 0x33; // 读取目标位置
            break;
        case S_SPOS:
            cmd[i++] = 0x34; // 读取实时设定目标位置
            break;
        case S_VEL:
            cmd[i++] = 0x35; // 读取实时转速
            break;
        case S_CPOS:
            cmd[i++] = 0x36; // 读取实时位置
            break;
        case S_PERR:
            cmd[i++] = 0x37; // 读取位置误差
            break;
        case S_VBAT:
            cmd[i++] = 0x38; // 读取多圈编码器电池电压（Y42）
            break;
        case S_TEMP:
            cmd[i++] = 0x39; // 读取实时温度（Y42）
            break;
        case S_FLAG:
            cmd[i++] = 0x3A; // 读取状态标志位
            break;
        case S_OFLAG:
            cmd[i++] = 0x3B; // 读取回零状态标志位
            break;
        case S_OAF:
            cmd[i++] = 0x3C; // 读取状态+回零标志位（Y42）
            break;
        case S_PIN:
            cmd[i++] = 0x3D; // 读取引脚状态（Y42）
            break;
        default:
            break;
    }

    cmd[i++] = 0x6B; // 校验字节

    for (j = 0; j < i; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/* ******************** 读取系统参数命令（多机） */

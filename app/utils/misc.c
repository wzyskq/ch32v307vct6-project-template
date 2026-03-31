#include "misc.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* 串口解析函数 ******************** */

/******************************************************************
 * \brief  解析 srlSigBuf 数据包内容
 * \note   在主循环中调用，当 srlSigFlg 为 1 时
 */
void misc_srlParse_sig(void)
{
    if (!srlSigFlg)
        return;

    // pass

    srlSigFlg = 0;
    if (srlReFlag)
        serial_printf(SRL_RESRL, "> Signal\n");
}

/******************************************************************
 * \brief  解析 srlPkgBuf 数据包内容
 * \note   在主循环中调用，当 srlPkgFlg 为 1 时
 */
void misc_srlParse_pkg(void)
{
    if (!srlPkgFlg)
        return;

    srlPkgFlg = 0;
    if (srlReFlag)
        serial_printf(SRL_RESRL, "> Package\n");
}

/******************************************************************
 * \brief  解析 srlPidBuf 数据包内容
 * \note   在主循环中调用，当 srlPidFlg 为 1 时
 */
void misc_srlParse_pid(void)
{
    if (!srlPidFlg)
        return;

    u8 len = srlPidBuf[0];
    if (len < 4) { // 至少要有一个字节
        srlPidFlg = 0;
        return;
    }

    // // 数据实际从 srlPidBuf[1] 开始
    // char type = srlPidBuf[1];
    // u8 num    = srlPidBuf[2] - '0';
    // float v   = strtof((char *)&srlPidBuf[4], NULL); // 从第4个字节开始转换为浮点数

    // e.g. {Pn 0.05}

    // switch (type) {
    //     case 'P':
    //         pidValue[num].Kp = v;
    //         break;
    //     case 'I':
    //         pidValue[num].Ki = v;
    //         break;
    //     case 'D':
    //         pidValue[num].Kd = v;
    //         break;
    //     default:
    //         break;
    // }

    srlPidFlg = 0;
    if (srlReFlag)
        serial_printf(SRL_RESRL, "> PID\n");
}

/******************************************************************
 * \brief  解析 srlCmdBuf 数据包内容
 * \note   在主循环中调用，当 srlCmdFlg 为 1 时
 * \note   支持的命令：
 *            debug -i/-o              打开/关闭调试模式
 *            srl -r <x> <str>         向串口x发送字符串
 *            led -i/-o/-t <x>         打开/关闭/切换 LEDx
 *            svo -p/-s <t> <c> <val>  设置指定时钟舵机位置/速度
 * \note   Q&A：
 *            Q1: 为什么要用传递指针 rCmd？
 *            A1: 若直接将 strmatch_s 的值赋给 cCmd，一旦第一个条件不满足，其值会直接变成 NULL，导致后续判断无法进行
 */
void misc_srlParse_cmd(void)
{
    if (!srlCmdFlg)
        return;

    u8 *cCmd    = srlCmdBuf + 1; // 正文指针
    u8 errLayer = 0;             // 错误层级，0 表示无错误，1 表示第一层错误，依此类推
    s16 arg     = 0;

    if (srlReFlag)
        serial_printf(SRL_RESRL, "SRL:  CMD: %s\n", srlCmdBuf + 1);

    if (strmatch_s("debug", cCmd, &cCmd)) {
        if (strmatch_s("-i", cCmd, &cCmd))
            srlReFlag = 1;
        else if (strmatch_s("-o", cCmd, &cCmd))
            srlReFlag = 0;
        else
            errLayer = 2;

    } else if (strmatch_s("srl", cCmd, &cCmd)) {
        if (strmatch_s("-r", cCmd, &cCmd)) {
            arg = (u8)strtof(cCmd, &cCmd);
            serial_printf(arg - 1, "%s\r\n", cCmd + 1);
        } else
            errLayer = 2;

    } else if (strmatch_s("led", cCmd, &cCmd)) {
        if (strmatch_s("-i", cCmd, &cCmd)) {
            arg = strtof(cCmd, NULL);
            led_on(arg);
        } else if (strmatch_s("-o", cCmd, &cCmd)) {
            arg = strtof(cCmd, NULL);
            led_off(arg);
        } else if (strmatch_s("-t", cCmd, &cCmd)) {
            arg = strtof(cCmd, NULL);
            led_turn(arg);
        } else
            errLayer = 2;

    } else if (strmatch_s("sm", cCmd, &cCmd)) {
        if (strmatch_s("--debug", cCmd, &cCmd)) {
            if (strmatch_s("-i", cCmd, &cCmd))
                zdtReFlag = 1;
            else if (strmatch_s("-o", cCmd, &cCmd))
                zdtReFlag = 0;
            else
                errLayer = 2;
        } else if (strmatch_s("-v", cCmd, &cCmd)) {
            u8 addr   = (u8)strtof(cCmd, &cCmd);
            float vel = (float)strtof(cCmd, &cCmd);
            u8 dir    = (vel > 0) ? 0 : 1; // 根据速度正负自动判断方向
            u8 acc    = (u8)strtof(cCmd, &cCmd);
            vel *= 10; // 开启了缩小十倍，实际输入单位为 0.1 RPM
            // while (zdtTvFlg); // 等待清零
            Emm_V5_Vel_Control(ZDT_SRL, addr, dir, (u16)ABS(vel), acc, false);
        } else if (strmatch_s("-p", cCmd, &cCmd)) {
            u8 addr   = (u8)strtof(cCmd, &cCmd);
            float vel = (float)strtof(cCmd, &cCmd);
            u8 acc    = (u8)strtof(cCmd, &cCmd);
            s32 pos   = (s32)strtof(cCmd, &cCmd);
            vel *= 10;                  // 开启了缩小十倍，实际输入单位为 0.1 RPM
            u8 dir = (pos > 0) ? 0 : 1; // 根据位置正负自动判断方向
            // while (zdtTvFlg);           // 等待清零
            Emm_V5_Pos_Control(ZDT_SRL, addr, dir, (u16)ABS(vel), acc, (u32)ABS(pos), true, false);
        } else if (strmatch_s("-o", cCmd, &cCmd)) {
            u8 addr = (u8)strtof(cCmd, &cCmd);
            // while (zdtTvFlg); // 等待清零
            Emm_V5_Stop_Now(ZDT_SRL, addr, false);
        } else if (strmatch_s("--oso", cCmd, &cCmd)) {
            u8 addr = (u8)strtof(cCmd, &cCmd);
            // while (zdtTvFlg); // 等待清零
            Emm_V5_Origin_Set_O(ZDT_SRL, addr, true);
        } else if (strmatch_s("--otr", cCmd, &cCmd)) {
            u8 addr = (u8)strtof(cCmd, &cCmd);
            // while (zdtTvFlg); // 等待清零
            Emm_V5_Origin_Trigger_Return(ZDT_SRL, addr, 1, false); // 1 为单圈方向回零
        } else
            errLayer = 2;

    } else if (strmatch_s("svo", cCmd, &cCmd)) {
        u8 t = 0, c = 0;
        if (strmatch_s("-p", cCmd, &cCmd)) {
            t   = (u8)strtof(cCmd, &cCmd); // 获取舵机时钟
            c   = (u8)strtof(cCmd, &cCmd); // 获取舵机通道
            arg = strtof(cCmd, NULL);      // 获取位置
            servo_set_pos(t, c, arg);      // 设置舵机位置
        } else if (strmatch_s("-s", cCmd, &cCmd)) {
            t   = (u8)strtof(cCmd, &cCmd); // 获取舵机时钟
            c   = (u8)strtof(cCmd, &cCmd); // 获取舵机通道
            arg = strtof(cCmd, NULL);      // 获取速度
            servo_set_spd(t, c, arg);      // 设置舵机速度
        } else if (strmatch_s("-d", cCmd, &cCmd)) {
            t   = (u8)strtof(cCmd, &cCmd);   // 获取舵机时钟
            c   = (u8)strtof(cCmd, &cCmd);   // 获取舵机通道
            arg = strtof(cCmd, NULL);        // 获取占空比
            timer_pwmOut_setDuty(t, c, arg); // 直接设置占空比
        } else
            errLayer = 2;

    } else
        errLayer = 1;

    srlCmdFlg = 0;

    if (srlReFlag && errLayer)
        serial_printf(SRL_RESRL, "SRL:  Err at layer %d\n", errLayer);
}

/* ******************** 串口解析函数 */

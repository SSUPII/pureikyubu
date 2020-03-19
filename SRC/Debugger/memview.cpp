// memory (data) view
#include "pch.h"

static const char *hexbyte(uint32_t addr)
{
    static char buf[0x10];

    // check address
    uint32_t pa = MEMEffectiveToPhysical(addr, 0);

    if (mi.ram)
    {
        if (pa != -1)
        {
            if (pa < RAMSIZE)
            {
                sprintf_s (buf, sizeof(buf), "%02X", mi.ram[pa]);
                return buf;
            }
        }
    }
    return "??";
}

static const char *charbyte(uint32_t addr)
{
    static char buf[0x10];
    buf[0] = '.'; buf[1] = 0;

    // check address
    uint32_t pa = MEMEffectiveToPhysical(addr, 0);

    if (mi.ram && pa != -1)
    {
        if (pa < RAMSIZE)
        {
            uint8_t data = mi.ram[pa];
            if ((data >= 32) && (data <= 255)) sprintf_s(buf, sizeof(buf), "%c\0", data);
            return buf;
        }
    }
    return "?";
}

void con_update_dump_window()
{
    con_fill_line(wind.data_y);
    con_attr(0, 3);
    if(wind.focus == WDATA) con_printf_at(0, wind.data_y, "\x1%c\x1f", ConColor::WHITE);
    con_attr(0, 3);
    con_print_at(2, wind.data_y, "F2");
    con_printf_at(6, wind.data_y, 
        " phys:%08X stack:%08X sda1:%08X sda2:%08X", 
        MEMEffectiveToPhysical(con.data, 0), SP, SDA1, SDA2
    );
    con_attr(7, 0);

    for(int row=0; row<wind.data_h-1; row++)
    {
        int col;
        static char buf[16];

        con_printf_at(0, wind.data_y + row + 1, "%08X", con.data + row * 16);

        for(col=0; col<8; col++)
            con_print_at(10 + col * 3, wind.data_y + row + 1, hexbyte(con.data + row * 16 + col));

        for(col=0; col<8; col++)
            con_print_at(35 + col * 3, wind.data_y + row + 1, hexbyte(con.data + row * 16 + col + 8));

        for(col=0; col<16; col++)
            con_print_at(60 + col, wind.data_y + row + 1, charbyte(con.data + row * 16 + col));
    }
}

void con_data_key(char ascii, int vkey, int ctrl)
{
    UNREFERENCED_PARAMETER(ascii);
    UNREFERENCED_PARAMETER(ctrl);

    switch(vkey)
    {
        case VK_HOME:
            con.data = (uint32_t)(1<<31);
            break;
        case VK_END:
            con.data = (RAMSIZE | (1<<31)) - ((wind.data_h - 1) * 16);
            break;
        case VK_NEXT:
            con.data += ((wind.data_h - 1) * 16);
            break;
        case VK_PRIOR:
            con.data -= ((wind.data_h - 1) * 16);
            break;
        case VK_UP:
            con.data -= 16;
            break;
        case VK_DOWN:
            con.data += 16;
            break;
        case VK_RETURN:
            con_memedit();
            break;
    }

    con.update |= CON_UPDATE_DATA;
}

// ---------------------------------------------------------------------------
// memory editing mode

static uint8_t   *medbuf;   // 16*H buffer
static uint8_t   *oldbuf;   // old 16*H buffer
static uint8_t   *medmask;  // mask 16*H buffer
static uint32_t  medaddr;   // current edit address
static uint8_t   mednibl;   // current nibble (0=both, 1=hi, 2=low)
static BOOL      medmode;   // 0=byte edit, 1=ascii edit

static void con_memedit_draw_hdr()
{
    con_fill_line(wind.data_y);
    con_attr(0, 3);
    if(wind.focus == WDATA) con_printf_at(0, wind.data_y, "\x1%c\x1f", ConColor::WHITE);
    con_attr(0, 3);
    con_print_at(2, wind.data_y, "F2");
    con_printf_at(6, wind.data_y, 
        " addr:%08X ofs:%-3i Keys: Esc, ^Enter, Tab, Home, End", medaddr, medaddr - con.data
    );
    con_attr(7, 0);
    con_blt_region(wind.data_y, 1);
}

static void con_memedit_draw()
{
    for(int row=0; row<wind.data_h-1; row++)
    {
        int col;

        for(col=0; col<8; col++)
        {
            uint8_t m = medmask[16*row + col];
            uint8_t c = medbuf[16*row + col];
            if(m & 0xf0)
                {con_printf_at(10 + col * 3, wind.data_y + row + 1, "\x1%c%01X", ConColor::BPUR, c >> 4);}
            else
                {con_printf_at(10 + col * 3, wind.data_y + row + 1, "\x1%c%01X", ConColor::NORM, c >> 4);}
            if(m & 0x0f)
                {con_printf_at(11 + col * 3, wind.data_y + row + 1, "\x1%c%01X", ConColor::BPUR, c & 0x0f);}
            else
                {con_printf_at(11 + col * 3, wind.data_y + row + 1, "\x1%c%01X", ConColor::NORM, c & 0x0f);}
        }

        for(col=0; col<8; col++)
        {
            uint8_t m = medmask[16*row + col + 8];
            uint8_t c = medbuf[16*row + col + 8];
            if(m & 0xf0)
                {con_printf_at(35 + col * 3, wind.data_y + row + 1, "\x1%c%01X", ConColor::BPUR, c >> 4);}
            else
                {con_printf_at(35 + col * 3, wind.data_y + row + 1, "\x1%c%01X", ConColor::NORM, c >> 4);}
            if(m & 0x0f)
                {con_printf_at(36 + col * 3, wind.data_y + row + 1, "\x1%c%01X", ConColor::BPUR, c & 0x0f);}
            else
                {con_printf_at(36 + col * 3, wind.data_y + row + 1, "\x1%c%01X", ConColor::NORM, c & 0x0f);}
        }

        for(col=0; col<16; col++)
        {
            uint8_t c = medbuf[16*row + col];
            if(c < ' ') c = '.';
            if(medmask[16*row + col])
                con_printf_at(60 + col, wind.data_y + row + 1, "\x1%c%c", ConColor::BPUR, c);
            else
                con_printf_at(60 + col, wind.data_y + row + 1, "\x1%c%c", ConColor::NORM, c);
        }
    }

    con_blt_region(wind.data_y, wind.data_h);
}

static void con_memedit_reload()
{
    // allocate temp buffer and load data
    medbuf  = (uint8_t*)malloc(16 * (wind.data_h-1));
    if(!medbuf) return;
    memset(medbuf, 0, 16 * (wind.data_h-1));
    if(mem.opened)
    {
        uint32_t ea = con.data;
        for(int i=0; i<16 * (wind.data_h-1); i++, ea++)
        {
            uint32_t pa = MEMEffectiveToPhysical(ea, 0), val;
            if(pa != 0xffffffff) MEMReadByte(ea, &val);
            else val = 0;
            medbuf[i] = (uint8_t)val;
        }
    }

    // save old
    oldbuf = (uint8_t*)malloc(16 * (wind.data_h-1));
    if(!oldbuf) return;
    memcpy(oldbuf, medbuf, 16 * (wind.data_h-1));

    // nibble mask buffer
    medmask = (uint8_t*)malloc(16 * (wind.data_h-1));
    if(!medmask) return;
    memset(medmask, 0, 16 * (wind.data_h-1));
}

static void con_memedit_cleanup()
{
    if(medbuf)
    {
        free(medbuf);
        medbuf = NULL;
    }
    if(oldbuf)
    {
        free(oldbuf);
        oldbuf = NULL;
    }
    if(medmask)
    {
        free(medmask);
        medmask = NULL;
    }
}

static void con_memedit_apply()
{
    if(!mem.opened) return;

    uint32_t ea = con.data;

    for(int row=0; row<wind.data_h-1; row++)
    {
        for(int col=0; col<16; col++, ea++)
        {
            uint32_t pa = MEMEffectiveToPhysical(ea, 0);
            if(pa != 0xffffffff)
            {
                uint32_t val = (medbuf[16*row+col] & medmask[16*row+col]) |
                          (oldbuf[16*row+col] & ~medmask[16*row+col]) ;
                MEMWriteByte(ea, val);
            }
        }
    }

    con_memedit_cleanup();
    con_memedit_reload();
    con_memedit_draw();
}

static void con_memedit_cur()
{
    int x, y, ofs = medaddr - con.data;

    y = wind.data_y + ofs / 16 + 1;

    if(!medmode)
    {
        x = 10 + 3 * (ofs % 16);
        if(ofs % 16 >= 8) x++;
        if(mednibl == 2) x++;
    }
    else x = 60 + (ofs % 16);

    con_cursorxy(x, y);
}

static void con_memedit_key(uint8_t ascii)
{
    if(!medmode)
    {
        int num, ofs;

        if((ascii >= '0') && (ascii <= '9')) num = ascii - '0';
        else if((ascii >= 'a') && (ascii <= 'f')) num = 0xa + ascii - 'a';
        else if((ascii >= 'A') && (ascii <= 'F')) num = 0xa + ascii - 'A';
        else return;

        if(mednibl == 1)            // high
        {
            ofs = medaddr - con.data;
            mednibl = 2;
            medbuf[ofs] &= 0x0f;
            medbuf[ofs] |= num << 4;
            if((oldbuf[ofs] >> 4) != (medbuf[ofs] >> 4))
            {
                medmask[ofs] |= 0xf0;
            }
            else medmask[ofs] &= ~0xf0;
        }
        else if(mednibl == 2)       // low
        {
            ofs = medaddr - con.data;
            if((medaddr + 1) < (con.data + 16*(wind.data_h-1)))
            {
                medaddr++;
                mednibl = 1;
            }
            medbuf[ofs] &= 0xf0;
            medbuf[ofs] |= num;
            if((oldbuf[ofs] & 0xf) != (medbuf[ofs] & 0xf))
            {
                medmask[ofs] |= 0x0f;
            }
            else medmask[ofs] &= ~0x0f;
        }
    }
    else
    {
        medbuf[medaddr - con.data] = ascii;
        medmask[medaddr - con.data] |= 0xff;
        if(!((medaddr + 1) >= (con.data + 16*(wind.data_h-1)))) medaddr++;
    }
    con_memedit_draw();
}

// editing input loop
void con_memedit()
{
    int             ofs;
    INPUT_RECORD    record;
    DWORD           count;

    // clear variables
    medmode = 0;
    medaddr = con.data;
    mednibl = 1;

    con_memedit_reload();
    con_memedit_draw_hdr();
    con_memedit_draw();
    
    for(;;)
    {
        con_memedit_cur();
        {
            count = 0;
            while(!count)
            ReadConsoleInput(con.input, &record, 1, &count);

            if(record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown)
            {
                uint8_t ascii = record.Event.KeyEvent.uChar.AsciiChar;
                int     vcode = record.Event.KeyEvent.wVirtualKeyCode;
                int     ctrl  = record.Event.KeyEvent.dwControlKeyState;

                switch(vcode)
                {
                    case VK_UP:
                        if((medaddr - 16) < con.data) break;
                        medaddr -= 16;
                        con_memedit_draw_hdr();
                        break;
                    case VK_DOWN:
                        if((medaddr + 16) >= (con.data + 16*(wind.data_h-1))) break;
                        medaddr += 16;
                        con_memedit_draw_hdr();
                        break;
                    case VK_RIGHT:
                        if(!medmode)
                        {
                            if(mednibl == 1)
                            {
                                mednibl = 2;
                            }
                            else
                            {
                                if((medaddr + 1) >= (con.data + 16*(wind.data_h-1))) break;
                                medaddr++;
                                mednibl = 1;
                            }
                        }
                        else
                        {
                            mednibl = 0;
                            if((medaddr + 1) >= (con.data + 16*(wind.data_h-1))) break;
                            medaddr++;
                        }
                        con_memedit_draw_hdr();
                        break;
                    case VK_LEFT:
                        if(!medmode)
                        {
                            if(mednibl == 2)
                            {
                                mednibl = 1;
                            }
                            else
                            {
                                if((medaddr - 1) < con.data) break;
                                medaddr--;
                                mednibl = 2;
                            }
                        }
                        else
                        {
                            mednibl = 0;
                            if((medaddr - 1) < con.data) break;
                            medaddr--;
                        }
                        con_memedit_draw_hdr();
                        break;
                    case VK_TAB:
                        medmode ^= 1;
                        mednibl = (!medmode) ? 1 : 0;
                        break;
                    case VK_HOME:
                        ofs = (medaddr - con.data) & ~15;
                        medaddr = con.data + ofs;
                        mednibl = (!medmode) ? 1 : 0;
                        con_memedit_draw_hdr();
                        break;
                    case VK_END:
                        ofs = ((medaddr - con.data) & ~15) + 15;
                        medaddr = con.data + ofs;
                        mednibl = (!medmode) ? 1 : 0;
                        con_memedit_draw_hdr();
                        break;
                    case VK_BACK:
                        #define OFS (medaddr - con.data)
                        if(!medmode)
                        {
                            if(mednibl == 1)        // high
                            {
                                if((medaddr - 1) < con.data) break;
                                medaddr--;
                                medbuf[OFS] &= 0xf0;
                                medbuf[OFS] |= (oldbuf[OFS] & 0x0f);
                                medmask[OFS] &= 0xf0;
                                mednibl = 2;
                            }
                            else                    // low
                            {
                                if(medaddr == (con.data + 16*(wind.data_h-1) - 1))
                                {
                                    if(medmask[OFS] & 0x0f)
                                    {
                                        medbuf[OFS] &= 0xf0;
                                        medbuf[OFS] |= (oldbuf[OFS] & 0x0f);
                                        medmask[OFS] &= 0xf0;
                                        con_memedit_draw();
                                        con_memedit_draw_hdr();
                                        break;
                                    }
                                }
                                medbuf[OFS] &= 0x0f;
                                medbuf[OFS] |= (oldbuf[OFS] & 0xf0);
                                medmask[OFS] &= 0x0f;
                                mednibl = 1;
                            }
                        }
                        else
                        {
                            mednibl = 0;
                            if((medaddr - 1) < con.data) break;
                            if(medaddr != (con.data + 16*(wind.data_h-1) - 1))
                            {
                                medaddr--;
                            }
                            if( (medaddr == (con.data + 16*(wind.data_h-1) - 1)) && 
                                (medmask[OFS] == 0) )
                            {
                                medaddr--;
                            }
                            medbuf[OFS] = oldbuf[OFS];
                            medmask[OFS] = 0;
                        }
                        con_memedit_draw();
                        con_memedit_draw_hdr();
                        break;
                    case VK_RETURN:
                        if(ctrl & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
                        {
                            con_memedit_apply();
                        }
                        break;
                    case VK_ESCAPE:
                        con_memedit_cleanup();
                        con_cursorxy(roll.editpos + 2, wind.edit_y);
                        con.update |= CON_UPDATE_DATA;
                        con_refresh();
                        return;
                    default:
                        if(ascii >= ' ' && mem.opened)
                        {
                            con_memedit_key(ascii);
                            con_memedit_draw_hdr();
                        }
                }
            }
        }
    }
}

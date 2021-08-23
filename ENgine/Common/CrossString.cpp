
#include "CrossString.h"
#include "Utils.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef PC

#include "Windows.h"
#include "Winuser.h"

#endif

#include "Core/Core.h"

std::map<wchar_t, int> String::unicode_upper2lower;
std::map<wchar_t, int> String::unicode_lower2upper;

void String::Init()
{
    unicode_upper2lower.clear();
    
	unicode_upper2lower.insert(pair<wchar_t, int>(0x0041, 0x0061));
	unicode_upper2lower[0x0042] = 0x0062;
	unicode_upper2lower[0x0043] = 0x0063;
	unicode_upper2lower[0x0044] = 0x0064;
	unicode_upper2lower[0x0045] = 0x0065;
	unicode_upper2lower[0x0046] = 0x0066;
	unicode_upper2lower[0x0047] = 0x0067;
	unicode_upper2lower[0x0048] = 0x0068;
	unicode_upper2lower[0x0049] = 0x0069;
	unicode_upper2lower[0x004A] = 0x006A;
	unicode_upper2lower[0x004B] = 0x006B;
	unicode_upper2lower[0x004C] = 0x006C;
	unicode_upper2lower[0x004D] = 0x006D;
	unicode_upper2lower[0x004E] = 0x006E;
	unicode_upper2lower[0x004F] = 0x006F;
	unicode_upper2lower[0x0050] = 0x0070;
	unicode_upper2lower[0x0051] = 0x0071;
	unicode_upper2lower[0x0052] = 0x0072;
	unicode_upper2lower[0x0053] = 0x0073;
	unicode_upper2lower[0x0054] = 0x0074;
	unicode_upper2lower[0x0055] = 0x0075;
	unicode_upper2lower[0x0056] = 0x0076;
	unicode_upper2lower[0x0057] = 0x0077;
	unicode_upper2lower[0x0058] = 0x0078;
	unicode_upper2lower[0x0059] = 0x0079;
	unicode_upper2lower[0x005A] = 0x007A;
	unicode_upper2lower[0x00C0] = 0x00E0;
	unicode_upper2lower[0x00C1] = 0x00E1;
	unicode_upper2lower[0x00C2] = 0x00E2;
	unicode_upper2lower[0x00C3] = 0x00E3;
	unicode_upper2lower[0x00C4] = 0x00E4;
	unicode_upper2lower[0x00C5] = 0x00E5;
	unicode_upper2lower[0x00C6] = 0x00E6;
	unicode_upper2lower[0x00C7] = 0x00E7;
	unicode_upper2lower[0x00C8] = 0x00E8;
	unicode_upper2lower[0x00C9] = 0x00E9;
	unicode_upper2lower[0x00CA] = 0x00EA;
	unicode_upper2lower[0x00CB] = 0x00EB;
	unicode_upper2lower[0x00CC] = 0x00EC;
	unicode_upper2lower[0x00CD] = 0x00ED;
	unicode_upper2lower[0x00CE] = 0x00EE;
	unicode_upper2lower[0x00CF] = 0x00EF;
	unicode_upper2lower[0x00D0] = 0x00F0;
	unicode_upper2lower[0x00D1] = 0x00F1;
	unicode_upper2lower[0x00D2] = 0x00F2;
	unicode_upper2lower[0x00D3] = 0x00F3;
	unicode_upper2lower[0x00D4] = 0x00F4;
	unicode_upper2lower[0x00D5] = 0x00F5;
	unicode_upper2lower[0x00D6] = 0x00F6;
	unicode_upper2lower[0x00D8] = 0x00F8;
	unicode_upper2lower[0x00D9] = 0x00F9;
	unicode_upper2lower[0x00DA] = 0x00FA;
	unicode_upper2lower[0x00DB] = 0x00FB;
	unicode_upper2lower[0x00DC] = 0x00FC;
	unicode_upper2lower[0x00DD] = 0x00FD;
	unicode_upper2lower[0x00DE] = 0x00FE;
	unicode_upper2lower[0x0100] = 0x0101;
	unicode_upper2lower[0x0102] = 0x0103;
	unicode_upper2lower[0x0104] = 0x0105;
	unicode_upper2lower[0x0106] = 0x0107;
	unicode_upper2lower[0x0108] = 0x0109;
	unicode_upper2lower[0x010A] = 0x010B;
	unicode_upper2lower[0x010C] = 0x010D;
	unicode_upper2lower[0x010E] = 0x010F;
	unicode_upper2lower[0x0110] = 0x0111;
	unicode_upper2lower[0x0112] = 0x0113;
	unicode_upper2lower[0x0114] = 0x0115;
	unicode_upper2lower[0x0116] = 0x0117;
	unicode_upper2lower[0x0118] = 0x0119;
	unicode_upper2lower[0x011A] = 0x011B;
	unicode_upper2lower[0x011C] = 0x011D;
	unicode_upper2lower[0x011E] = 0x011F;
	unicode_upper2lower[0x0120] = 0x0121;
	unicode_upper2lower[0x0122] = 0x0123;
	unicode_upper2lower[0x0124] = 0x0125;
	unicode_upper2lower[0x0126] = 0x0127;
	unicode_upper2lower[0x0128] = 0x0129;
	unicode_upper2lower[0x012A] = 0x012B;
	unicode_upper2lower[0x012C] = 0x012D;
	unicode_upper2lower[0x012E] = 0x012F;
	unicode_upper2lower[0x0130] = 0x0069;
	unicode_upper2lower[0x0132] = 0x0133;
	unicode_upper2lower[0x0134] = 0x0135;
	unicode_upper2lower[0x0136] = 0x0137;
	unicode_upper2lower[0x0139] = 0x013A;
	unicode_upper2lower[0x013B] = 0x013C;
	unicode_upper2lower[0x013D] = 0x013E;
	unicode_upper2lower[0x013F] = 0x0140;
	unicode_upper2lower[0x0141] = 0x0142;
	unicode_upper2lower[0x0143] = 0x0144;
	unicode_upper2lower[0x0145] = 0x0146;
	unicode_upper2lower[0x0147] = 0x0148;
	unicode_upper2lower[0x014A] = 0x014B;
	unicode_upper2lower[0x014C] = 0x014D;
	unicode_upper2lower[0x014E] = 0x014F;
	unicode_upper2lower[0x0150] = 0x0151;
	unicode_upper2lower[0x0152] = 0x0153;
	unicode_upper2lower[0x0154] = 0x0155;
	unicode_upper2lower[0x0156] = 0x0157;
	unicode_upper2lower[0x0158] = 0x0159;
	unicode_upper2lower[0x015A] = 0x015B;
	unicode_upper2lower[0x015C] = 0x015D;
	unicode_upper2lower[0x015E] = 0x015F;
	unicode_upper2lower[0x0160] = 0x0161;
	unicode_upper2lower[0x0162] = 0x0163;
	unicode_upper2lower[0x0164] = 0x0165;
	unicode_upper2lower[0x0166] = 0x0167;
	unicode_upper2lower[0x0168] = 0x0169;
	unicode_upper2lower[0x016A] = 0x016B;
	unicode_upper2lower[0x016C] = 0x016D;
	unicode_upper2lower[0x016E] = 0x016F;
	unicode_upper2lower[0x0170] = 0x0171;
	unicode_upper2lower[0x0172] = 0x0173;
	unicode_upper2lower[0x0174] = 0x0175;
	unicode_upper2lower[0x0176] = 0x0177;
	unicode_upper2lower[0x0178] = 0x00FF;
	unicode_upper2lower[0x0179] = 0x017A;
	unicode_upper2lower[0x017B] = 0x017C;
	unicode_upper2lower[0x017D] = 0x017E;
	unicode_upper2lower[0x0181] = 0x0253;
	unicode_upper2lower[0x0182] = 0x0183;
	unicode_upper2lower[0x0184] = 0x0185;
	unicode_upper2lower[0x0186] = 0x0254;
	unicode_upper2lower[0x0187] = 0x0188;
	unicode_upper2lower[0x018A] = 0x0257;
	unicode_upper2lower[0x018B] = 0x018C;
	unicode_upper2lower[0x018E] = 0x0258;
	unicode_upper2lower[0x018F] = 0x0259;
	unicode_upper2lower[0x0190] = 0x025B;
	unicode_upper2lower[0x0191] = 0x0192;
	unicode_upper2lower[0x0193] = 0x0260;
	unicode_upper2lower[0x0194] = 0x0263;
	unicode_upper2lower[0x0196] = 0x0269;
	unicode_upper2lower[0x0197] = 0x0268;
	unicode_upper2lower[0x0198] = 0x0199;
	unicode_upper2lower[0x019C] = 0x026f;
	unicode_upper2lower[0x019D] = 0x0272;
	unicode_upper2lower[0x019F] = 0x0275;
	unicode_upper2lower[0x01A0] = 0x01A1;
	unicode_upper2lower[0x01A2] = 0x01A3;
	unicode_upper2lower[0x01A4] = 0x01A5;
	unicode_upper2lower[0x01A7] = 0x01A8;
	unicode_upper2lower[0x01A9] = 0x0283;
	unicode_upper2lower[0x01AC] = 0x01AD;
	unicode_upper2lower[0x01AE] = 0x0288;
	unicode_upper2lower[0x01AF] = 0x01B0;
	unicode_upper2lower[0x01B1] = 0x028A;
	unicode_upper2lower[0x01B2] = 0x028B;
	unicode_upper2lower[0x01B3] = 0x01B4;
	unicode_upper2lower[0x01B5] = 0x01B6;
	unicode_upper2lower[0x01B7] = 0x0292;
	unicode_upper2lower[0x01B8] = 0x01B9;
	unicode_upper2lower[0x01BC] = 0x01BD;
	unicode_upper2lower[0x01C4] = 0x01C6;
	unicode_upper2lower[0x01C5] = 0x01C6;
	unicode_upper2lower[0x01C7] = 0x01C9;
	unicode_upper2lower[0x01C8] = 0x01C9;
	unicode_upper2lower[0x01CA] = 0x01CC;
	unicode_upper2lower[0x01CB] = 0x01CC;
	unicode_upper2lower[0x01CD] = 0x01CE;
	unicode_upper2lower[0x01CF] = 0x01D0;
	unicode_upper2lower[0x01D1] = 0x01D2;
	unicode_upper2lower[0x01D3] = 0x01D4;
	unicode_upper2lower[0x01D5] = 0x01D6;
	unicode_upper2lower[0x01D7] = 0x01D8;
	unicode_upper2lower[0x01D9] = 0x01DA;
	unicode_upper2lower[0x01DB] = 0x01DC;
	unicode_upper2lower[0x01DE] = 0x01DF;
	unicode_upper2lower[0x01E0] = 0x01E1;
	unicode_upper2lower[0x01E2] = 0x01E3;
	unicode_upper2lower[0x01E4] = 0x01E5;
	unicode_upper2lower[0x01E6] = 0x01E7;
	unicode_upper2lower[0x01E8] = 0x01E9;
	unicode_upper2lower[0x01EA] = 0x01EB;
	unicode_upper2lower[0x01EC] = 0x01ED;
	unicode_upper2lower[0x01EE] = 0x01EF;
	unicode_upper2lower[0x01F1] = 0x01F3;
	unicode_upper2lower[0x01F4] = 0x01F5;
	unicode_upper2lower[0x01FA] = 0x01FB;
	unicode_upper2lower[0x01FC] = 0x01FD;
	unicode_upper2lower[0x01FE] = 0x01FF;
	unicode_upper2lower[0x0200] = 0x0201;
	unicode_upper2lower[0x0202] = 0x0203;
	unicode_upper2lower[0x0204] = 0x0205;
	unicode_upper2lower[0x0206] = 0x0207;
	unicode_upper2lower[0x0208] = 0x0209;
	unicode_upper2lower[0x020A] = 0x020B;
	unicode_upper2lower[0x020C] = 0x020D;
	unicode_upper2lower[0x020E] = 0x020F;
	unicode_upper2lower[0x0210] = 0x0211;
	unicode_upper2lower[0x0212] = 0x0213;
	unicode_upper2lower[0x0214] = 0x0215;
	unicode_upper2lower[0x0216] = 0x0217;
	unicode_upper2lower[0x0386] = 0x03AC;
	unicode_upper2lower[0x0388] = 0x03AD;
	unicode_upper2lower[0x0389] = 0x03AE;
	unicode_upper2lower[0x038A] = 0x03AF;
	unicode_upper2lower[0x038C] = 0x03CC;
	unicode_upper2lower[0x038E] = 0x03CD;
	unicode_upper2lower[0x038F] = 0x03CE;
	unicode_upper2lower[0x0391] = 0x03B1;
	unicode_upper2lower[0x0392] = 0x03B2;
	unicode_upper2lower[0x0393] = 0x03B3;
	unicode_upper2lower[0x0394] = 0x03B4;
	unicode_upper2lower[0x0395] = 0x03B5;
	unicode_upper2lower[0x0396] = 0x03B6;
	unicode_upper2lower[0x0397] = 0x03B7;
	unicode_upper2lower[0x0398] = 0x03B8;
	unicode_upper2lower[0x0399] = 0x03B9;
	unicode_upper2lower[0x039A] = 0x03BA;
	unicode_upper2lower[0x039B] = 0x03BB;
	unicode_upper2lower[0x039C] = 0x03BC;
	unicode_upper2lower[0x039D] = 0x03BD;
	unicode_upper2lower[0x039E] = 0x03BE;
	unicode_upper2lower[0x039F] = 0x03BF;
	unicode_upper2lower[0x03A0] = 0x03C0;
	unicode_upper2lower[0x03A1] = 0x03C1;
	unicode_upper2lower[0x03A3] = 0x03C3;
	unicode_upper2lower[0x03A4] = 0x03C4;
	unicode_upper2lower[0x03A5] = 0x03C5;
	unicode_upper2lower[0x03A6] = 0x03C6;
	unicode_upper2lower[0x03A7] = 0x03C7;
	unicode_upper2lower[0x03A8] = 0x03C8;
	unicode_upper2lower[0x03A9] = 0x03C9;
	unicode_upper2lower[0x03AA] = 0x03CA;
	unicode_upper2lower[0x03AB] = 0x03CB;
	unicode_upper2lower[0x03E2] = 0x03E3;
	unicode_upper2lower[0x03E4] = 0x03E5;
	unicode_upper2lower[0x03E6] = 0x03E7;
	unicode_upper2lower[0x03E8] = 0x03E9;
	unicode_upper2lower[0x03EA] = 0x03EB;
	unicode_upper2lower[0x03EC] = 0x03ED;
	unicode_upper2lower[0x03EE] = 0x03EF;
	unicode_upper2lower[0x0401] = 0x0451;
	unicode_upper2lower[0x0402] = 0x0452;
	unicode_upper2lower[0x0403] = 0x0453;
	unicode_upper2lower[0x0404] = 0x0454;
	unicode_upper2lower[0x0405] = 0x0455;
	unicode_upper2lower[0x0406] = 0x0456;
	unicode_upper2lower[0x0407] = 0x0457;
	unicode_upper2lower[0x0408] = 0x0458;
	unicode_upper2lower[0x0409] = 0x0459;
	unicode_upper2lower[0x040A] = 0x045A;
	unicode_upper2lower[0x040B] = 0x045B;
	unicode_upper2lower[0x040C] = 0x045C;
	unicode_upper2lower[0x040E] = 0x045E;
	unicode_upper2lower[0x040F] = 0x045F;
	unicode_upper2lower[0x0410] = 0x0430;
	unicode_upper2lower[0x0411] = 0x0431;
	unicode_upper2lower[0x0412] = 0x0432;
	unicode_upper2lower[0x0413] = 0x0433;
	unicode_upper2lower[0x0414] = 0x0434;
	unicode_upper2lower[0x0415] = 0x0435;
	unicode_upper2lower[0x0416] = 0x0436;
	unicode_upper2lower[0x0417] = 0x0437;
	unicode_upper2lower[0x0418] = 0x0438;
	unicode_upper2lower[0x0419] = 0x0439;
	unicode_upper2lower[0x041A] = 0x043A;
	unicode_upper2lower[0x041B] = 0x043B;
	unicode_upper2lower[0x041C] = 0x043C;
	unicode_upper2lower[0x041D] = 0x043D;
	unicode_upper2lower[0x041E] = 0x043E;
	unicode_upper2lower[0x041F] = 0x043F;
	unicode_upper2lower[0x0420] = 0x0440;
	unicode_upper2lower[0x0421] = 0x0441;
	unicode_upper2lower[0x0422] = 0x0442;
	unicode_upper2lower[0x0423] = 0x0443;
	unicode_upper2lower[0x0424] = 0x0444;
	unicode_upper2lower[0x0425] = 0x0445;
	unicode_upper2lower[0x0426] = 0x0446;
	unicode_upper2lower[0x0427] = 0x0447;
	unicode_upper2lower[0x0428] = 0x0448;
	unicode_upper2lower[0x0429] = 0x0449;
	unicode_upper2lower[0x042A] = 0x044A;
	unicode_upper2lower[0x042B] = 0x044B;
	unicode_upper2lower[0x042C] = 0x044C;
	unicode_upper2lower[0x042D] = 0x044D;
	unicode_upper2lower[0x042E] = 0x044E;
	unicode_upper2lower[0x042F] = 0x044F;
	unicode_upper2lower[0x0460] = 0x0461;
	unicode_upper2lower[0x0462] = 0x0463;
	unicode_upper2lower[0x0464] = 0x0465;
	unicode_upper2lower[0x0466] = 0x0467;
	unicode_upper2lower[0x0468] = 0x0469;
	unicode_upper2lower[0x046A] = 0x046B;
	unicode_upper2lower[0x046C] = 0x046D;
	unicode_upper2lower[0x046E] = 0x046F;
	unicode_upper2lower[0x0470] = 0x0471;
	unicode_upper2lower[0x0472] = 0x0473;
	unicode_upper2lower[0x0474] = 0x0475;
	unicode_upper2lower[0x0476] = 0x0477;
	unicode_upper2lower[0x0478] = 0x0479;
	unicode_upper2lower[0x047A] = 0x047B;
	unicode_upper2lower[0x047C] = 0x047D;
	unicode_upper2lower[0x047E] = 0x047F;
	unicode_upper2lower[0x0480] = 0x0481;
	unicode_upper2lower[0x0490] = 0x0491;
	unicode_upper2lower[0x0492] = 0x0493;
	unicode_upper2lower[0x0494] = 0x0495;
	unicode_upper2lower[0x0496] = 0x0497;
	unicode_upper2lower[0x0498] = 0x0499;
	unicode_upper2lower[0x049A] = 0x049B;
	unicode_upper2lower[0x049C] = 0x049D;
	unicode_upper2lower[0x049E] = 0x049F;
	unicode_upper2lower[0x04A0] = 0x04A1;
	unicode_upper2lower[0x04A2] = 0x04A3;
	unicode_upper2lower[0x04A4] = 0x04A5;
	unicode_upper2lower[0x04A6] = 0x04A7;
	unicode_upper2lower[0x04A8] = 0x04A9;
	unicode_upper2lower[0x04AA] = 0x04AB;
	unicode_upper2lower[0x04AC] = 0x04AD;
	unicode_upper2lower[0x04AE] = 0x04AF;
	unicode_upper2lower[0x04B0] = 0x04B1;
	unicode_upper2lower[0x04B2] = 0x04B3;
	unicode_upper2lower[0x04B4] = 0x04B5;
	unicode_upper2lower[0x04B6] = 0x04B7;
	unicode_upper2lower[0x04B8] = 0x04B9;
	unicode_upper2lower[0x04BA] = 0x04BB;
	unicode_upper2lower[0x04BC] = 0x04BD;
	unicode_upper2lower[0x04BE] = 0x04BF;
	unicode_upper2lower[0x04C1] = 0x04C2;
	unicode_upper2lower[0x04C3] = 0x04C4;
	unicode_upper2lower[0x04C7] = 0x04C8;
	unicode_upper2lower[0x04CB] = 0x04CC;
	unicode_upper2lower[0x04D0] = 0x04D1;
	unicode_upper2lower[0x04D2] = 0x04D3;
	unicode_upper2lower[0x04D4] = 0x04D5;
	unicode_upper2lower[0x04D6] = 0x04D7;
	unicode_upper2lower[0x04D8] = 0x04D9;
	unicode_upper2lower[0x04DA] = 0x04DB;
	unicode_upper2lower[0x04DC] = 0x04DD;
	unicode_upper2lower[0x04DE] = 0x04DF;
	unicode_upper2lower[0x04E0] = 0x04E1;
	unicode_upper2lower[0x04E2] = 0x04E3;
	unicode_upper2lower[0x04E4] = 0x04E5;
	unicode_upper2lower[0x04E6] = 0x04E7;
	unicode_upper2lower[0x04E8] = 0x04E9;
	unicode_upper2lower[0x04EA] = 0x04EB;
	unicode_upper2lower[0x04EE] = 0x04EF;
	unicode_upper2lower[0x04F0] = 0x04F1;
	unicode_upper2lower[0x04F2] = 0x04F3;
	unicode_upper2lower[0x04F4] = 0x04F5;
	unicode_upper2lower[0x04F8] = 0x04F9;
	unicode_upper2lower[0x0531] = 0x0561;
	unicode_upper2lower[0x0532] = 0x0562;
	unicode_upper2lower[0x0533] = 0x0563;
	unicode_upper2lower[0x0534] = 0x0564;
	unicode_upper2lower[0x0535] = 0x0565;
	unicode_upper2lower[0x0536] = 0x0566;
	unicode_upper2lower[0x0537] = 0x0567;
	unicode_upper2lower[0x0538] = 0x0568;
	unicode_upper2lower[0x0539] = 0x0569;
	unicode_upper2lower[0x053A] = 0x056A;
	unicode_upper2lower[0x053B] = 0x056B;
	unicode_upper2lower[0x053C] = 0x056C;
	unicode_upper2lower[0x053D] = 0x056D;
	unicode_upper2lower[0x053E] = 0x056E;
	unicode_upper2lower[0x053F] = 0x056F;
	unicode_upper2lower[0x0540] = 0x0570;
	unicode_upper2lower[0x0541] = 0x0571;
	unicode_upper2lower[0x0542] = 0x0572;
	unicode_upper2lower[0x0543] = 0x0573;
	unicode_upper2lower[0x0544] = 0x0574;
	unicode_upper2lower[0x0545] = 0x0575;
	unicode_upper2lower[0x0546] = 0x0576;
	unicode_upper2lower[0x0547] = 0x0577;
	unicode_upper2lower[0x0548] = 0x0578;
	unicode_upper2lower[0x0549] = 0x0579;
	unicode_upper2lower[0x054A] = 0x057A;
	unicode_upper2lower[0x054B] = 0x057B;
	unicode_upper2lower[0x054C] = 0x057C;
	unicode_upper2lower[0x054D] = 0x057D;
	unicode_upper2lower[0x054E] = 0x057E;
	unicode_upper2lower[0x054F] = 0x057F;
	unicode_upper2lower[0x0550] = 0x0580;
	unicode_upper2lower[0x0551] = 0x0581;
	unicode_upper2lower[0x0552] = 0x0582;
	unicode_upper2lower[0x0553] = 0x0583;
	unicode_upper2lower[0x0554] = 0x0584;
	unicode_upper2lower[0x0555] = 0x0585;
	unicode_upper2lower[0x0556] = 0x0586;
	unicode_upper2lower[0x10A0] = 0x10D0;
	unicode_upper2lower[0x10A1] = 0x10D1;
	unicode_upper2lower[0x10A2] = 0x10D2;
	unicode_upper2lower[0x10A3] = 0x10D3;
	unicode_upper2lower[0x10A4] = 0x10D4;
	unicode_upper2lower[0x10A5] = 0x10D5;
	unicode_upper2lower[0x10A6] = 0x10D6;
	unicode_upper2lower[0x10A7] = 0x10D7;
	unicode_upper2lower[0x10A8] = 0x10D8;
	unicode_upper2lower[0x10A9] = 0x10D9;
	unicode_upper2lower[0x10AA] = 0x10DA;
	unicode_upper2lower[0x10AB] = 0x10DB;
	unicode_upper2lower[0x10AC] = 0x10DC;
	unicode_upper2lower[0x10AD] = 0x10DD;
	unicode_upper2lower[0x10AE] = 0x10DE;
	unicode_upper2lower[0x10AF] = 0x10DF;
	unicode_upper2lower[0x10B0] = 0x10E0;
	unicode_upper2lower[0x10B1] = 0x10E1;
	unicode_upper2lower[0x10B2] = 0x10E2;
	unicode_upper2lower[0x10B3] = 0x10E3;
	unicode_upper2lower[0x10B4] = 0x10E4;
	unicode_upper2lower[0x10B5] = 0x10E5;
	unicode_upper2lower[0x10B6] = 0x10E6;
	unicode_upper2lower[0x10B7] = 0x10E7;
	unicode_upper2lower[0x10B8] = 0x10E8;
	unicode_upper2lower[0x10B9] = 0x10E9;
	unicode_upper2lower[0x10BA] = 0x10EA;
	unicode_upper2lower[0x10BB] = 0x10EB;
	unicode_upper2lower[0x10BC] = 0x10EC;
	unicode_upper2lower[0x10BD] = 0x10ED;
	unicode_upper2lower[0x10BE] = 0x10EE;
	unicode_upper2lower[0x10BF] = 0x10EF;
	unicode_upper2lower[0x10C0] = 0x10F0;
	unicode_upper2lower[0x10C1] = 0x10F1;
	unicode_upper2lower[0x10C2] = 0x10F2;
	unicode_upper2lower[0x10C3] = 0x10F3;
	unicode_upper2lower[0x10C4] = 0x10F4;
	unicode_upper2lower[0x10C5] = 0x10F5;
	unicode_upper2lower[0x1E00] = 0x1E01;
	unicode_upper2lower[0x1E02] = 0x1E03;
	unicode_upper2lower[0x1E04] = 0x1E05;
	unicode_upper2lower[0x1E06] = 0x1E07;
	unicode_upper2lower[0x1E08] = 0x1E09;
	unicode_upper2lower[0x1E0A] = 0x1E0B;
	unicode_upper2lower[0x1E0C] = 0x1E0D;
	unicode_upper2lower[0x1E0E] = 0x1E0F;
	unicode_upper2lower[0x1E10] = 0x1E11;
	unicode_upper2lower[0x1E12] = 0x1E13;
	unicode_upper2lower[0x1E14] = 0x1E15;
	unicode_upper2lower[0x1E16] = 0x1E17;
	unicode_upper2lower[0x1E18] = 0x1E19;
	unicode_upper2lower[0x1E1A] = 0x1E1B;
	unicode_upper2lower[0x1E1C] = 0x1E1D;
	unicode_upper2lower[0x1E1E] = 0x1E1F;
	unicode_upper2lower[0x1E20] = 0x1E21;
	unicode_upper2lower[0x1E22] = 0x1E23;
	unicode_upper2lower[0x1E24] = 0x1E25;
	unicode_upper2lower[0x1E26] = 0x1E27;
	unicode_upper2lower[0x1E28] = 0x1E29;
	unicode_upper2lower[0x1E2A] = 0x1E2B;
	unicode_upper2lower[0x1E2C] = 0x1E2D;
	unicode_upper2lower[0x1E2E] = 0x1E2F;
	unicode_upper2lower[0x1E30] = 0x1E31;
	unicode_upper2lower[0x1E32] = 0x1E33;
	unicode_upper2lower[0x1E34] = 0x1E35;
	unicode_upper2lower[0x1E36] = 0x1E37;
	unicode_upper2lower[0x1E38] = 0x1E39;
	unicode_upper2lower[0x1E3A] = 0x1E3B;
	unicode_upper2lower[0x1E3C] = 0x1E3D;
	unicode_upper2lower[0x1E3E] = 0x1E3F;
	unicode_upper2lower[0x1E40] = 0x1E41;
	unicode_upper2lower[0x1E42] = 0x1E43;
	unicode_upper2lower[0x1E44] = 0x1E45;
	unicode_upper2lower[0x1E46] = 0x1E47;
	unicode_upper2lower[0x1E48] = 0x1E49;
	unicode_upper2lower[0x1E4A] = 0x1E4B;
	unicode_upper2lower[0x1E4C] = 0x1E4D;
	unicode_upper2lower[0x1E4E] = 0x1E4F;
	unicode_upper2lower[0x1E50] = 0x1E51;
	unicode_upper2lower[0x1E52] = 0x1E53;
	unicode_upper2lower[0x1E54] = 0x1E55;
	unicode_upper2lower[0x1E56] = 0x1E57;
	unicode_upper2lower[0x1E58] = 0x1E59;
	unicode_upper2lower[0x1E5A] = 0x1E5B;
	unicode_upper2lower[0x1E5C] = 0x1E5D;
	unicode_upper2lower[0x1E5E] = 0x1E5F;
	unicode_upper2lower[0x1E60] = 0x1E61;
	unicode_upper2lower[0x1E62] = 0x1E63;
	unicode_upper2lower[0x1E64] = 0x1E65;
	unicode_upper2lower[0x1E66] = 0x1E67;
	unicode_upper2lower[0x1E68] = 0x1E69;
	unicode_upper2lower[0x1E6A] = 0x1E6B;
	unicode_upper2lower[0x1E6C] = 0x1E6D;
	unicode_upper2lower[0x1E6E] = 0x1E6F;
	unicode_upper2lower[0x1E70] = 0x1E71;
	unicode_upper2lower[0x1E72] = 0x1E73;
	unicode_upper2lower[0x1E74] = 0x1E75;
	unicode_upper2lower[0x1E76] = 0x1E77;
	unicode_upper2lower[0x1E78] = 0x1E79;
	unicode_upper2lower[0x1E7A] = 0x1E7B;
	unicode_upper2lower[0x1E7C] = 0x1E7D;
	unicode_upper2lower[0x1E7E] = 0x1E7F;
	unicode_upper2lower[0x1E80] = 0x1E81;
	unicode_upper2lower[0x1E82] = 0x1E83;
	unicode_upper2lower[0x1E84] = 0x1E85;
	unicode_upper2lower[0x1E86] = 0x1E87;
	unicode_upper2lower[0x1E88] = 0x1E89;
	unicode_upper2lower[0x1E8A] = 0x1E8B;
	unicode_upper2lower[0x1E8C] = 0x1E8D;
	unicode_upper2lower[0x1E8E] = 0x1E8F;
	unicode_upper2lower[0x1E90] = 0x1E91;
	unicode_upper2lower[0x1E92] = 0x1E93;
	unicode_upper2lower[0x1E94] = 0x1E95;
	unicode_upper2lower[0x1EA0] = 0x1EA1;
	unicode_upper2lower[0x1EA2] = 0x1EA3;
	unicode_upper2lower[0x1EA4] = 0x1EA5;
	unicode_upper2lower[0x1EA6] = 0x1EA7;
	unicode_upper2lower[0x1EA8] = 0x1EA9;
	unicode_upper2lower[0x1EAA] = 0x1EAB;
	unicode_upper2lower[0x1EAC] = 0x1EAD;
	unicode_upper2lower[0x1EAE] = 0x1EAF;
	unicode_upper2lower[0x1EB0] = 0x1EB1;
	unicode_upper2lower[0x1EB2] = 0x1EB3;
	unicode_upper2lower[0x1EB4] = 0x1EB5;
	unicode_upper2lower[0x1EB6] = 0x1EB7;
	unicode_upper2lower[0x1EB8] = 0x1EB9;
	unicode_upper2lower[0x1EBA] = 0x1EBB;
	unicode_upper2lower[0x1EBC] = 0x1EBD;
	unicode_upper2lower[0x1EBE] = 0x1EBF;
	unicode_upper2lower[0x1EC0] = 0x1EC1;
	unicode_upper2lower[0x1EC2] = 0x1EC3;
	unicode_upper2lower[0x1EC4] = 0x1EC5;
	unicode_upper2lower[0x1EC6] = 0x1EC7;
	unicode_upper2lower[0x1EC8] = 0x1EC9;
	unicode_upper2lower[0x1ECA] = 0x1ECB;
	unicode_upper2lower[0x1ECC] = 0x1ECD;
	unicode_upper2lower[0x1ECE] = 0x1ECF;
	unicode_upper2lower[0x1ED0] = 0x1ED1;
	unicode_upper2lower[0x1ED2] = 0x1ED3;
	unicode_upper2lower[0x1ED4] = 0x1ED5;
	unicode_upper2lower[0x1ED6] = 0x1ED7;
	unicode_upper2lower[0x1ED8] = 0x1ED9;
	unicode_upper2lower[0x1EDA] = 0x1EDB;
	unicode_upper2lower[0x1EDC] = 0x1EDD;
	unicode_upper2lower[0x1EDE] = 0x1EDF;
	unicode_upper2lower[0x1EE0] = 0x1EE1;
	unicode_upper2lower[0x1EE2] = 0x1EE3;
	unicode_upper2lower[0x1EE4] = 0x1EE5;
	unicode_upper2lower[0x1EE6] = 0x1EE7;
	unicode_upper2lower[0x1EE8] = 0x1EE9;
	unicode_upper2lower[0x1EEA] = 0x1EEB;
	unicode_upper2lower[0x1EEC] = 0x1EED;
	unicode_upper2lower[0x1EEE] = 0x1EEF;
	unicode_upper2lower[0x1EF0] = 0x1EF1;
	unicode_upper2lower[0x1EF2] = 0x1EF3;
	unicode_upper2lower[0x1EF4] = 0x1EF5;
	unicode_upper2lower[0x1EF6] = 0x1EF7;
	unicode_upper2lower[0x1EF8] = 0x1EF9;
	unicode_upper2lower[0x1F08] = 0x1F00;
	unicode_upper2lower[0x1F09] = 0x1F01;
	unicode_upper2lower[0x1F0A] = 0x1F02;
	unicode_upper2lower[0x1F0B] = 0x1F03;
	unicode_upper2lower[0x1F0C] = 0x1F04;
	unicode_upper2lower[0x1F0D] = 0x1F05;
	unicode_upper2lower[0x1F0E] = 0x1F06;
	unicode_upper2lower[0x1F0F] = 0x1F07;
	unicode_upper2lower[0x1F18] = 0x1F10;
	unicode_upper2lower[0x1F19] = 0x1F11;
	unicode_upper2lower[0x1F1A] = 0x1F12;
	unicode_upper2lower[0x1F1B] = 0x1F13;
	unicode_upper2lower[0x1F1C] = 0x1F14;
	unicode_upper2lower[0x1F1D] = 0x1F15;
	unicode_upper2lower[0x1F28] = 0x1F20;
	unicode_upper2lower[0x1F29] = 0x1F21;
	unicode_upper2lower[0x1F2A] = 0x1F22;
	unicode_upper2lower[0x1F2B] = 0x1F23;
    unicode_upper2lower[0x1F2C] = 0x1F24;
	unicode_upper2lower[0x1F2D] = 0x1F25;
	unicode_upper2lower[0x1F2E] = 0x1F26;
	unicode_upper2lower[0x1F2F] = 0x1F27;
	unicode_upper2lower[0x1F38] = 0x1F30;
	unicode_upper2lower[0x1F39] = 0x1F31;
	unicode_upper2lower[0x1F3A] = 0x1F32;
	unicode_upper2lower[0x1F3B] = 0x1F33;
	unicode_upper2lower[0x1F3C] = 0x1F34;
	unicode_upper2lower[0x1F3D] = 0x1F35;
	unicode_upper2lower[0x1F3E] = 0x1F36;
	unicode_upper2lower[0x1F3F] = 0x1F37;
	unicode_upper2lower[0x1F48] = 0x1F40;
	unicode_upper2lower[0x1F49] = 0x1F41;
	unicode_upper2lower[0x1F4A] = 0x1F42;
	unicode_upper2lower[0x1F4B] = 0x1F43;
	unicode_upper2lower[0x1F4C] = 0x1F44;
	unicode_upper2lower[0x1F4D] = 0x1F45;
	unicode_upper2lower[0x1F59] = 0x1F51;
	unicode_upper2lower[0x1F5B] = 0x1F53;
	unicode_upper2lower[0x1F5D] = 0x1F55;
	unicode_upper2lower[0x1F5F] = 0x1F57;
	unicode_upper2lower[0x1F68] = 0x1F60;
	unicode_upper2lower[0x1F69] = 0x1F61;
	unicode_upper2lower[0x1F6A] = 0x1F62;
	unicode_upper2lower[0x1F6B] = 0x1F63;
	unicode_upper2lower[0x1F6C] = 0x1F64;
	unicode_upper2lower[0x1F6D] = 0x1F65;
	unicode_upper2lower[0x1F6E] = 0x1F66;
	unicode_upper2lower[0x1F6F] = 0x1F67;
	unicode_upper2lower[0x1F88] = 0x1F80;
	unicode_upper2lower[0x1F89] = 0x1F81;
	unicode_upper2lower[0x1F8A] = 0x1F82;
	unicode_upper2lower[0x1F8B] = 0x1F83;
	unicode_upper2lower[0x1F8C] = 0x1F84;
	unicode_upper2lower[0x1F8D] = 0x1F85;
	unicode_upper2lower[0x1F8E] = 0x1F86;
	unicode_upper2lower[0x1F8F] = 0x1F87;
	unicode_upper2lower[0x1F98] = 0x1F90;
	unicode_upper2lower[0x1F99] = 0x1F91;
	unicode_upper2lower[0x1F9A] = 0x1F92;
	unicode_upper2lower[0x1F9B] = 0x1F93;
	unicode_upper2lower[0x1F9C] = 0x1F94;
	unicode_upper2lower[0x1F9D] = 0x1F95;
	unicode_upper2lower[0x1F9E] = 0x1F96;
	unicode_upper2lower[0x1F9F] = 0x1F97;
	unicode_upper2lower[0x1FA8] = 0x1FA0;
	unicode_upper2lower[0x1FA9] = 0x1FA1;
	unicode_upper2lower[0x1FAA] = 0x1FA2;
	unicode_upper2lower[0x1FAB] = 0x1FA3;
	unicode_upper2lower[0x1FAC] = 0x1FA4;
	unicode_upper2lower[0x1FAD] = 0x1FA5;
	unicode_upper2lower[0x1FAE] = 0x1FA6;
	unicode_upper2lower[0x1FAF] = 0x1FA7;
	unicode_upper2lower[0x1FB8] = 0x1FB0;
	unicode_upper2lower[0x1FB9] = 0x1FB1;
	unicode_upper2lower[0x1FD8] = 0x1FD0;
	unicode_upper2lower[0x1FD9] = 0x1FD1;
	unicode_upper2lower[0x1FE8] = 0x1FE0;
	unicode_upper2lower[0x1FE9] = 0x1FE1;
	unicode_upper2lower[0x24B6] = 0x24D0;
	unicode_upper2lower[0x24B7] = 0x24D1;
	unicode_upper2lower[0x24B8] = 0x24D2;
	unicode_upper2lower[0x24B9] = 0x24D3;
	unicode_upper2lower[0x24BA] = 0x24D4;
	unicode_upper2lower[0x24BB] = 0x24D5;
	unicode_upper2lower[0x24BC] = 0x24D6;
	unicode_upper2lower[0x24BD] = 0x24D7;
	unicode_upper2lower[0x24BE] = 0x24D8;
	unicode_upper2lower[0x24BF] = 0x24D9;
	unicode_upper2lower[0x24C0] = 0x24DA;
	unicode_upper2lower[0x24C1] = 0x24DB;
	unicode_upper2lower[0x24C2] = 0x24DC;
	unicode_upper2lower[0x24C3] = 0x24DD;
	unicode_upper2lower[0x24C4] = 0x24DE;
	unicode_upper2lower[0x24C5] = 0x24DF;
	unicode_upper2lower[0x24C6] = 0x24E0;
	unicode_upper2lower[0x24C7] = 0x24E1;
	unicode_upper2lower[0x24C8] = 0x24E2;
	unicode_upper2lower[0x24C9] = 0x24E3;
	unicode_upper2lower[0x24CA] = 0x24E4;
	unicode_upper2lower[0x24CB] = 0x24E5;
	unicode_upper2lower[0x24CC] = 0x24E6;
	unicode_upper2lower[0x24CD] = 0x24E7;
	unicode_upper2lower[0x24CE] = 0x24E8;
	unicode_upper2lower[0x24CF] = 0x24E9;
	unicode_upper2lower[0xFF21] = 0xFF41;
	unicode_upper2lower[0xFF22] = 0xFF42;
	unicode_upper2lower[0xFF23] = 0xFF43;
	unicode_upper2lower[0xFF24] = 0xFF44;
	unicode_upper2lower[0xFF25] = 0xFF45;
	unicode_upper2lower[0xFF26] = 0xFF46;
	unicode_upper2lower[0xFF27] = 0xFF47;
	unicode_upper2lower[0xFF28] = 0xFF48;
	unicode_upper2lower[0xFF29] = 0xFF49;
	unicode_upper2lower[0xFF2A] = 0xFF4A;
	unicode_upper2lower[0xFF2B] = 0xFF4B;
	unicode_upper2lower[0xFF2C] = 0xFF4C;
	unicode_upper2lower[0xFF2D] = 0xFF4D;
	unicode_upper2lower[0xFF2E] = 0xFF4E;
	unicode_upper2lower[0xFF2F] = 0xFF4F;
	unicode_upper2lower[0xFF30] = 0xFF50;
	unicode_upper2lower[0xFF31] = 0xFF51;
	unicode_upper2lower[0xFF32] = 0xFF52;
	unicode_upper2lower[0xFF33] = 0xFF53;
	unicode_upper2lower[0xFF34] = 0xFF54;
	unicode_upper2lower[0xFF35] = 0xFF55;
	unicode_upper2lower[0xFF36] = 0xFF56;
	unicode_upper2lower[0xFF37] = 0xFF57;
	unicode_upper2lower[0xFF38] = 0xFF58;
	unicode_upper2lower[0xFF39] = 0xFF59;
	unicode_upper2lower[0xFF3A] = 0xFF5A;

	//for(std::map<wchar_t, int>::const_iterator it  = unicode_upper2lower.begin();it != unicode_upper2lower.end();++it)
	{
		//unicode_lower2upper[it->second] = it->first;
	}
}

bool String::IsEmpty(const char * c)
{
	return !c || !c[0];
}

long String::Compare(const char * s1, const char * s2)
{ 
	if(!s1) s1 = "";
	if(!s2) s2 = "";
	
	char str1[512];
    Copy(str1, 512, s1);
    LowerCase(str1);

    char str2[512];
    Copy(str2, 512, s2);
    LowerCase(str2);

	return strcmp(str1, str2);	
}

bool String::IsEqual(const char * s1, const char * s2)
{ 
	return Compare(s1, s2) == 0;
}

bool String::IsEqual(const char * str1, long len1, const char * str2, long len2)
{
	if(len1 != len2)
	{
		return false;
	}

	for(; len1 > 0; len1--, str1++, str2++)
	{
		if(*str1 != *str2)
		{
			return false;
		}
	}
	return true;
}

void String::Copy(char * s1, int len, const char * s2)
{
	int size = strlen(s2)+1;
	int mv = size;
	if (size>len) mv = len-1;

	memcpy(s1, s2, mv);	

    if (size>len) s1[len-1] = 0;
}

void String::Cat(char * s1, int len, const char * s2)
{
	#ifdef PC
		strcat_s(s1, len,s2);
	#else
		strcat(s1, s2);
	#endif
	
}

void String::Printf(char * s1, int len, const char * format, ...)
{
	static char buffer[4096];
	va_list args;
	va_start(args, format);

	#ifdef PC
		_vsnprintf(buffer, sizeof(buffer) - 4, format, args);
	#else
		vsnprintf(buffer, sizeof(buffer) - 4, format, args);
	#endif

	va_end(args);

	Copy(s1, len, buffer);
}

void String::Scanf(const char * src, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	
	#ifdef PC
		// vsscanf(src, format, args);
	#else
		// sscanf(buffer, format, args);
	#endif
	
	va_end(args);
}

void String::RemoveSlashes(char* FullPath)
{
	static char buffer[1024];
	Copy(buffer, 1023, FullPath);

	int len = strlen(FullPath);
	int index=0;

	int skip = 0;

	for (int i=0;i<len;i++)
	{
		if (buffer[i] == '/' || buffer[i] == '\\')
		{
			skip++;
		}
		else
		{
			skip = 0;
		}

		if (skip<2)
		{
			FullPath[index] = buffer[i];
			index++;
		}
	}

	FullPath[index] = 0;
}

void String::ExtractFileName(const char* FullPath, char *FileName)
{
	int index = strlen(FullPath)-1;

	while(index>=0&&FullPath[index]!='\\'&&FullPath[index]!='/')
	{
		index--;
	}

	for(int i=(int)index+1;i<(int)strlen(FullPath);i++)
	{
		FileName[i-index-1]=FullPath[i];
	}

	FileName[strlen(FullPath)-index-1]=0;	
}

void String::ExtractPath(const char* FullPath, char *Path,bool retempty)
{
	int index = strlen(FullPath)-1;

	while(index>=0&&FullPath[index]!='\\'&&FullPath[index]!='/')
	{
		index--;
	}

	if (index>0)
	{	
		for(int i=0;i<=index;i++)
		{
			Path[i]=FullPath[i];
		}

		Path[index+1]=0;
	}
	else
	{
		if (retempty) Path[0] = 0;
	}	
}

void String::GetCropPath(const char* RelativePath,const char* FullPath, char* Path, int len)
{	
	if (FullPath[1] != ':')
	{
		Copy(Path, len, FullPath);

		return;
	}

	if (strlen(FullPath)<=strlen(RelativePath))
	{
		Path[0] = 0;

		return;
	}

	for(int i=(int)strlen(RelativePath);i<(int)strlen(FullPath);i++)
	{
		Path[i-strlen(RelativePath)]=FullPath[i];
	}

	Path[strlen(FullPath)-strlen(RelativePath)]=0;
}

void String::ExtractRoot(char* Path, char *root)
{
	char* pPath = Path;

	int index=0;

	root[0] = 0;

	// Skip along to the first option delimiter "/" or "-"
	while ( *pPath != '\0' && *pPath != '/' && *pPath != '\\' )
	{		
		root[index]=*pPath;
		pPath++;
		index++;
	}

	root[index] = '\0';
}

void String::ExtractRootPath(const char* path1,const char* path2,char* root)
{
	char rt[512];
	int len = strlen(path1);
	if (len < strlen(path2)) len = strlen(path2);

	rt[0] = 0;

	for (int i=0;i<len;i++)
	{
		if (path1[i] == path2[i])
		{
			rt[i] = path1[i];
			rt[i+1] = 0;
		}
	}

	ExtractPath(rt,root,false);	
}

void String::RemoveFirstChar(char* str)
{
	for(int i=0;i<(int)strlen(str)-1;i++)
	{
		str[i]=str[i+1];
	}

	str[strlen(str)-1]=0;
}

void String::FixSlashes(char * str)
{
	int len = strlen(str);

	for(int i=0;i<len;i++)
	{				
		if (str[i] == '\\')
		{
			str[i]='/';			
		}

		if (i!=0 && str[i] == '/')
		{
			if (str[i-1] == '/')
			{
				for(int j=i;j<len;j++)
				{
					str[j] = str[j+1];
				}

				len--;
				i--;
			}
		}
	}
}

void String::ExtractExctention(const char* str, char* ext, int ext_lenght)
{	
	ext[0] = 0;

	int  count = 0;
	int index = strlen(str)-1;

	while (index>0&&str[index]!='.')
	{	
		count++;
		
		if (ext_lenght == count) return;

		index--;
	}	

	int i=index+1;

	for (i=index+1;i<(int)strlen(str);i++)
	{
		ext[i-index-1]=str[i];
	}	

	ext[i-index-1]=0;
}

void String::RemoveExctention(char* str)
{
	int index = strlen(str) - 1;

	while (index>=0&&str[index] != '.')
	{
		index--;
	}	

	if (index>0)
	{
		str[index] = 0;
	}	
}

bool String::CompareAlphapedOrder(const char* str1, const char* str2)
{	
	int bytes = 0;
	
	int w1 = 0;
	int len1 = strlen(str1);
	int index1 = 0;
	
	int w2 = 0;
	int len2 = strlen(str2);
	int index2 = 0;
	
	std::map<wchar_t,int>::iterator it;

	bool finished = false;
	int stage = 0;

	while (!finished)
	{
		if (stage == 0)
		{
			if (BuildUtf16fromUtf8(str1[index1], bytes, w1))
			{				
				it = unicode_upper2lower.find(w1);

				if (it != unicode_upper2lower.end())
				{
					w1 = it->second;
				}

				stage++;
			}
			else
			{
				if (index1 + 1 >= len1)
				{
					//core->TraceTo("app", "%s are corrupted", str1);
					return true;
				}
			}
			
			index1++;			
		}
		else
		if (stage == 1)
		{
			if (BuildUtf16fromUtf8(str2[index2], bytes, w2))
			{				
				it = unicode_upper2lower.find(w2);

				if (it != unicode_upper2lower.end())
				{
					w2 = it->second;
				}
	
				stage++;
			}
			else
			{
				if (index2 + 1 >= len2)
				{
					//core->TraceTo("app", "%s are corrupted", str2);
					break;
				}
			}

			index2++;					
		}
		else
		{
			if (w1 != w2)
			{
				if (w1 > 128 && w2 < 128)
				{
					return true;
				}

				if (w2 > 128 && w1 < 128)
				{
					return false;
				}

				return (w1 < w2);
			}

			if (index1 >= len1 && index2 < len2)
			{				
				return true;
			}			

			if (index2 >= len2 && index1 < len1)
			{				
				break;
			}	

			stage = 0;
		}
	}	

	return false;
}

bool String::IsSubstr(const char* str, const char* substr)
{
	if ( String::IsEmpty(substr) )
		return false;
	int steps = (int)strlen(str)-(int)strlen(substr) + 1;
	for (int i=0;i< steps;i++)
	{
		bool SubstFinded = true;
		for (int j=0;j<(int)strlen(substr);j++)
		{
			if (str[i+j]!=substr[j])
			{
				SubstFinded = false;
				break;
			}
		}

		if (SubstFinded) return true;
	}

	return false;
}

void String::InsertStr(char* str, const char* substr, int position)
{
	static char tmp_str[512];
	Copy(tmp_str,511,str);

	int index = 0;

	for (int i=0;i<strlen(tmp_str)+1;i++)
	{
		if (i == position)
		{
			for (int j=0;j<strlen(substr);j++)
			{			
				str[index] = substr[j];
				index++;
			}
		}

		str[index] = tmp_str[i];
		index++;
	}	
}

void String::ExtractNameNumber(const char* str, char* wo_n_str, int len, int& number)
{
	Copy(wo_n_str,len,str);
	number = 0;

	int index = strlen(str)-1;

	if (index == 0) return;

	char buffer[512];

	while (index>=0 && str[index] >= '0' && str[index] <= '9')
	{
		buffer[strlen(str)-1 - index] = str[index];
		index--;
	}

	buffer[strlen(str)-1 - index] = 0;

	int ln = (int)(strlen(buffer) * 0.5f);
	for (int i = 0; i<ln;i++)
	{
		int tmp = buffer[i];
		buffer[i] = buffer[strlen(buffer)-1];
		buffer[strlen(buffer)-1] = tmp;
	}

	if (strlen(buffer)==0) return;

	sscanf (buffer, "%i", &number);	
	wo_n_str[index+1] = 0;	
}

void String::LowerCase(char* str)
{
	int len = strlen(str);

	for (int i=0;i<len;i++)
	{
		str[i] = tolower(str[i]);
	}	
}

void String::LowerCase(string& str)
{
	string text = str;	
	str.clear();

	int w = 0;
	int bytes = 0;

	int len = text.size();

	string tmp;
	std::map<wchar_t,int>::iterator it;

 
	for (int i=0;i<len;i++)
	{											
		if (!BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}														

		it = unicode_upper2lower.find(w);

		if (it != unicode_upper2lower.end())
		{
			w = it->second;
		}

		BuildUtf8fromUtf16(w, tmp);
		str += tmp;		
	}		
}

void String::UpperCase(string& str)
{
	string text = str;	
	str.clear();

	int w = 0;
	int bytes = 0;

	int len = text.size();

	string tmp;
	std::map<wchar_t,int>::iterator it;

	for (int i=0;i<len;i++)
	{											
		if (!BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}														

		it = unicode_lower2upper.find(w);

		if (it != unicode_lower2upper.end())
		{
			w = it->second;
		}

		BuildUtf8fromUtf16(w, tmp);
		str += tmp;		
	}		
}

bool String::ParsePlatform(const char* in, char* out, int len, Platform platform)
{
	const char* key = strstr(in, "#platform");

	if (!key)
	{		
		return false;
	}

	const char* platform_str = "platforms/pc";

	switch (platform)
	{
		case platform_pc:
			platform_str = "platforms/pc";
		break;
		case platform_ios:
			platform_str = "platforms/ios";
		break;
		case platform_android:
			platform_str = "platforms/android";
		break;
		case platform_osx:
			platform_str = "platforms/osx";
		break;
		case platform_unix:
			platform_str = "platforms/unix";
		break;
	}

	char* ptr = out;
	int ln = key - in;

	for (int i=0; i<ln; i++)
	{
		*ptr = in[i];
		ptr++;
	}

	ln = strlen(platform_str);
	
	for (int i=0; i<ln; i++)
	{
		*ptr = platform_str[i];
		ptr++;
	}

	const char* ptr2 = key + strlen("#platform");

	ln = strlen(ptr2);

	for (int i=0; i<ln; i++)
	{
		*ptr = *ptr2;
		ptr++;
		ptr2++;
	}

	*ptr = 0;

	return true;
}

void String::EscapeChars(const char* in, char* out, int len)
{
	int index = 0;
	int in_len = strlen(in);
	
	for	(int i=0;i<in_len;i++)
	{
		char c = in[i];

		if (c == ' ')
		{
			out[index] = '%';
			out[index+1] = '2';
			out[index+2] = '0';
			index += 3;
		}
		else		
		if (c == ':')
		{
			out[index] = '%';
			out[index+1] = '3';			
			out[index+2] = 'A';	
			index += 3;
		}
		else
		if (c == '#')
		{
			out[index] = '%';
			out[index+1] = '2';			
			out[index+2] = '3';	
			index += 3;
		}
		else
		{
			out[index] = c;
			index ++;
		}
	}

	out[index] = 0;
}

void String::ReplaceAll(string& str, const char* find,const char* replace)
{	
	string find_str = find;
	string replace_str = replace;

    string wsRet;
    wsRet.reserve(str.length());

    size_t start_pos = 0, pos;
    
	while((pos = str.find(find, start_pos)) != string::npos)
	{
        wsRet += str.substr(start_pos, pos - start_pos);
        wsRet += replace_str;
        pos += find_str.length();
        start_pos = pos;
    }

    wsRet += str.substr(start_pos);
    str.swap(wsRet);
}

bool String::GetAbsalutPath(string& sPath,const string& sBasePath)
{
    String::FixSlashes(&sPath[0]);
    if(sPath.find(":") == string::npos)
    {
        if(String::IsSubstr(sPath.c_str(),"../"))
        {
            string find_str = "../",sSubPath;
            unsigned start_pos = 0, pos,count = 1;
            while((pos = sPath.find(find_str, start_pos)) != string::npos)
            {
                sSubPath += sPath.substr(start_pos, pos - start_pos);
                pos += find_str.length();
                start_pos = pos;
                ++count;
            }
            sSubPath += sPath.substr(start_pos);
            string sBuffer = sBasePath;
            String::FixSlashes(&sBuffer[0]);
            int i = sBuffer.size() - 1;
            for(; i >= 0;--i)
            {
                if(sBuffer[i] == '/')--count;
                if(count  == 0 )break;
            }
            if(count > 0)
                return false;
            else
                sPath = sBuffer.substr(0,i) + '/' + sSubPath;
        }
        else
        {
            string sBuffer;
            sBuffer.resize(sBasePath.size());
            String::ExtractPath(sBasePath.c_str(),&sBuffer[0],false);
            String::FixSlashes(&sBuffer[0]);
            sPath  = string(sBuffer.c_str())  + sPath;
        }
    }
    return  true;
}

void String::ASCIItoUtf8(std::string& dest, const char* src)
{
    dest.clear();

    static const short utf[256] =
    {
        0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
        0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,
        0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
        0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,
        0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,
        0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,
        0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
        0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x402,0x403,0x201a,0x453,0x201e,
        0x2026,0x2020,0x2021,0x20ac,0x2030,0x409,0x2039,0x40a,0x40c,0x40b,0x40f,0x452,
        0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,0,0x2122,0x459,0x203a,0x45a,
        0x45c,0x45b,0x45f,0xa0,0x40e,0x45e,0x408,0xa4,0x490,0xa6,0xa7,0x401,0xa9,0x404,
        0xab,0xac,0xad,0xae,0x407,0xb0,0xb1,0x406,0x456,0x491,0xb5,0xb6,0xb7,0x451,
        0x2116,0x454,0xbb,0x458,0x405,0x455,0x457,0x410,0x411,0x412,0x413,0x414,0x415,
        0x416,0x417,0x418,0x419,0x41a,0x41b,0x41c,0x41d,0x41e,0x41f,0x420,0x421,0x422,
        0x423,0x424,0x425,0x426,0x427,0x428,0x429,0x42a,0x42b,0x42c,0x42d,0x42e,0x42f,
        0x430,0x431,0x432,0x433,0x434,0x435,0x436,0x437,0x438,0x439,0x43a,0x43b,0x43c,
        0x43d,0x43e,0x43f,0x440,0x441,0x442,0x443,0x444,0x445,0x446,0x447,0x448,0x449,
        0x44a,0x44b,0x44c,0x44d,0x44e,0x44f
    };

    int len = strlen(src);	

    for(int i=0;i<len;i++)
    {
        long c = utf[(unsigned char)src[i]];

        if (c < 0x80)
        {
            dest += c;
        }
        else		
        {
            dest += c >> 6 | 0xc0;
            dest += c & 0x3f | 0x80;
        }
    }
}

void String::Utf16toUtf8(std::string& dest, const wchar_t* src)
{
    dest.clear();

    int len = wcslen(src);

    for (int i = 0; i < len; i++) 
    {
        int c = src[i];

        if (c < (1 << 7))
        {
            dest += c;
        }
        else
            if (c < (1 << 11))
            {
                dest += ((c >> 6) | 0xcC0);
                dest += ((c & 0x3F) | 0x80);
            }
            else
                if (c < (1 << 16))
                {
                    dest += ((c >> 12) | 0xE0);
                    dest += (((c >> 6) & 0x3F) | 0x80);
                    dest += ((c & 0x3F) | 0x80);
                }
                else
                    if (c < (1 << 21))
                    {
                        dest += ((c >> 18) | 0xE0);
                        dest += (((c >> 12) & 0x3F) | 0x80);
                        dest += (((c >> 6) & 0x3F) | 0x80);
                        dest += ((c & 0x3F) | 0x80);
                    }
    }
}

void String::Utf8toUtf16(std::wstring& dest, const char* src)
{
    dest.clear();
    wchar_t w = 0;
    int bytes = 0;
    wchar_t err = L'?';

    int count = strlen(src);

    for (size_t i = 0; i < count; i++)
    {
        unsigned char c = (unsigned char)src[i];

        if (c <= 0x7f)
        {
            //first byte
            if (bytes)
            {
                dest.push_back(err);
                bytes = 0;
            }

            dest.push_back((wchar_t)c);
        }
        else
            if (c <= 0xbf)
            {//second/third/etc byte
                if (bytes)
                {
                    w = ((w << 6)|(c & 0x3f));
                    bytes--;
                    if (bytes == 0)
                        dest.push_back(w);
                }
                else
                {
                    dest.push_back(err);
                }
            }
            else
                if (c <= 0xdf)
                {//2byte sequence start
                    bytes = 1;
                    w = c & 0x1f;
                }
                else
                    if (c <= 0xef)
                    {//3byte sequence start
                        bytes = 2;
                        w = c & 0x0f;
                    }
                    else
                        if (c <= 0xf7)
                        {//3byte sequence start
                            bytes = 3;
                            w = c & 0x07;
                        }
                        else
                        {
                            dest.push_back(err);
                            bytes = 0;
                        }
    }

    if (bytes) dest.push_back(err);
}



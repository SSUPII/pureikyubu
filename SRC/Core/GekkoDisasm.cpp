// Gekko Disassembler.
#include "pch.h"

namespace Gekko
{
	// Not all simplified instructions are supported. Some of those presented in the manual are so simplified that they only confuse.
	std::string GekkoDisasm::SimplifiedInstruction(AnalyzeInfo* info, bool& simple, bool skipOperand[5])
	{
		static const char* t_cond[32] = {
			NULL, "lgt", "llt", NULL, "eq", "lge", "lle", NULL,
			"gt", NULL, NULL, NULL, "ge", NULL, NULL, NULL,
			"lt", NULL, NULL, NULL, "le", NULL, NULL, NULL,
			"ne", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		};

		char simplified[0x40] = { 0, };
		simple = false;

		// Trap

		if (info->instr == Instruction::twi && t_cond[info->paramBits[0]])
		{
			sprintf_s(simplified, sizeof(simplified) - 1, "tw%si", t_cond[info->paramBits[0]]);
			skipOperand[0] = true;
			simple = true;
			return simplified;
		}
		else if (info->instr == Instruction::twi && t_cond[info->paramBits[0]])
		{
			sprintf_s(simplified, sizeof(simplified) - 1, "tw%s", t_cond[info->paramBits[0]]);
			skipOperand[0] = true;
			simple = true;
			return simplified;
		}

		// Compare

		else if (info->instr == Instruction::cmp)
		{
			if (info->paramBits[0] == 0)
			{
				skipOperand[0] = true;
			}
			simple = true;
			return "cmpw";
		}
		else if (info->instr == Instruction::cmpi)
		{
			if (info->paramBits[0] == 0)
			{
				skipOperand[0] = true;
			}
			simple = true;
			return "cmpwi";
		}
		else if (info->instr == Instruction::cmpl)
		{
			if (info->paramBits[0] == 0)
			{
				skipOperand[0] = true;
			}
			simple = true;
			return "cmplw";
		}
		else if (info->instr == Instruction::cmpli)
		{
			if (info->paramBits[0] == 0)
			{
				skipOperand[0] = true;
			}
			simple = true;
			return "cmplwi";
		}

		// Add

		else if (info->instr == Instruction::addi && info->paramBits[1] == 0)
		{
			skipOperand[1] = true;
			simple = true;
			return "li";
		}
		else if (info->instr == Instruction::addis && info->paramBits[1] == 0)
		{
			skipOperand[1] = true;
			simple = true;
			return "lis";
		}
		else if (info->instr == Instruction::addic && info->Imm.Unsigned & 0x8000)
		{
			info->Imm.Unsigned = ~(info->Imm.Unsigned) + 1;
			info->param[2] = Param::Simm;
			simple = true;
			return "subic";
		}
		else if (info->instr == Instruction::addic_d && info->Imm.Unsigned & 0x8000)
		{
			info->Imm.Unsigned = ~(info->Imm.Unsigned) + 1;
			info->param[2] = Param::Simm;
			simple = true;
			return "subic.";
		}
		else if (info->instr == Instruction::addis && info->Imm.Unsigned & 0x8000)
		{
			info->Imm.Unsigned = ~(info->Imm.Unsigned) + 1;
			info->param[2] = Param::Simm;
			simple = true;
			return "subis";
		}

		// Bcx

		// Condition Register

		// Mtcrf

		// Special-purpose reg

		// Tbrs

		// Nop (Ori)

		return "";
	}

	std::string GekkoDisasm::HexToStr(uint8_t value)
	{
		char buf[0x10] = { 0, };
		sprintf_s(buf, sizeof(buf) - 1, "%02X", value);
		return std::string(buf);
	}

	std::string GekkoDisasm::HexToStr(uint16_t value)
	{
		char buf[0x10] = { 0, };
		sprintf_s(buf, sizeof(buf) - 1, "%04X", value);
		return std::string(buf);
	}

	std::string GekkoDisasm::HexToStr(uint32_t value)
	{
		char buf[0x10] = { 0, };
		sprintf_s(buf, sizeof(buf) - 1, "%08X", value);
		return std::string(buf);
	}

	std::string GekkoDisasm::InstrToString(AnalyzeInfo* info)
	{
		// This switch-case spent approximately 4200 Joules.
		switch (info->instr)
		{
			case Instruction::addi: return "addi";
			case Instruction::addis: return "addis";
			case Instruction::add: return "add";
			case Instruction::add_d: return "add.";
			case Instruction::addo: return "addo";
			case Instruction::addo_d: return "addo.";
			case Instruction::subf: return "subf";
			case Instruction::subf_d: return "subf.";
			case Instruction::subfo: return "subfo";
			case Instruction::subfo_d: return "subfo.";
			case Instruction::addic: return "addic";
			case Instruction::addic_d: return "addic.";
			case Instruction::subfic: return "subfic";
			case Instruction::addc: return "addc";
			case Instruction::addc_d: return "addc.";
			case Instruction::addco: return "addco";
			case Instruction::addco_d: return "addco.";
			case Instruction::subfc: return "subfc";
			case Instruction::subfc_d: return "subfc.";
			case Instruction::subfco: return "subfco";
			case Instruction::subfco_d: return "subfco.";
			case Instruction::adde: return "adde";
			case Instruction::adde_d: return "adde.";
			case Instruction::addeo: return "addeo";
			case Instruction::addeo_d: return "addeo.";
			case Instruction::subfe: return "subfe";
			case Instruction::subfe_d: return "subfe.";
			case Instruction::subfeo: return "subfeo";
			case Instruction::subfeo_d: return "subfeo.";
			case Instruction::addme: return "addme";
			case Instruction::addme_d: return "addme.";
			case Instruction::addmeo: return "addmeo";
			case Instruction::addmeo_d: return "addmeo.";
			case Instruction::subfme: return "subfme";
			case Instruction::subfme_d: return "subfme.";
			case Instruction::subfmeo: return "subfmeo";
			case Instruction::subfmeo_d: return "subfmeo.";
			case Instruction::addze: return "addze";
			case Instruction::addze_d: return "addze.";
			case Instruction::addzeo: return "addzeo";
			case Instruction::addzeo_d: return "addzeo.";
			case Instruction::subfze: return "subfze";
			case Instruction::subfze_d: return "subfze.";
			case Instruction::subfzeo: return "subfzeo";
			case Instruction::subfzeo_d: return "subfzeo.";
			case Instruction::neg: return "neg";
			case Instruction::neg_d: return "neg.";
			case Instruction::nego: return "nego";
			case Instruction::nego_d: return "nego.";
			case Instruction::mulli: return "mulli";
			case Instruction::mullw: return "mullw";
			case Instruction::mullw_d: return "mullw.";
			case Instruction::mullwo: return "mullwo";
			case Instruction::mullwo_d: return "mullwo.";
			case Instruction::mulhw: return "mulhw";
			case Instruction::mulhw_d: return "mulhw.";
			case Instruction::mulhwu: return "mulhwu";
			case Instruction::mulhwu_d: return "mulhwu.";
			case Instruction::divw: return "divw";
			case Instruction::divw_d: return "divw.";
			case Instruction::divwo: return "divwo";
			case Instruction::divwo_d: return "divwo.";
			case Instruction::divwu: return "divwu";
			case Instruction::divwu_d: return "divwu.";
			case Instruction::divwuo: return "divwuo";
			case Instruction::divwuo_d: return "divwuo.";
			case Instruction::cmpi: return "cmpi";
			case Instruction::cmp: return "cmp";
			case Instruction::cmpli: return "cmpli";
			case Instruction::cmpl: return "cmpl";
			case Instruction::andi_d: return "andi.";
			case Instruction::andis_d: return "andis.";
			case Instruction::ori: return "ori";
			case Instruction::oris: return "oris";
			case Instruction::xori: return "xori";
			case Instruction::xoris: return "xoris";
			case Instruction::_and: return "and";
			case Instruction::and_d: return "and.";
			case Instruction::_or: return "or";
			case Instruction::or_d: return "or.";
			case Instruction::_xor: return "xor";
			case Instruction::xor_d: return "xor.";
			case Instruction::nand: return "nand";
			case Instruction::nand_d: return "nand.";
			case Instruction::nor: return "nor";
			case Instruction::nor_d: return "nor.";
			case Instruction::eqv: return "eqv";
			case Instruction::eqv_d: return "eqv.";
			case Instruction::andc: return "andc";
			case Instruction::andc_d: return "andc.";
			case Instruction::orc: return "orc";
			case Instruction::orc_d: return "orc.";
			case Instruction::extsb: return "extsb";
			case Instruction::extsb_d: return "extsb.";
			case Instruction::extsh: return "extsh";
			case Instruction::extsh_d: return "extsh.";
			case Instruction::cntlzw: return "cntlzw";
			case Instruction::cntlzw_d: return "cntlzw.";
			case Instruction::rlwinm: return "rlwinm";
			case Instruction::rlwinm_d: return "rlwinm.";
			case Instruction::rlwnm: return "rlwnm";
			case Instruction::rlwnm_d: return "rlwnm.";
			case Instruction::rlwimi: return "rlwimi";
			case Instruction::rlwimi_d: return "rlwimi.";
			case Instruction::slw: return "slw";
			case Instruction::slw_d: return "slw.";
			case Instruction::srw: return "srw";
			case Instruction::srw_d: return "srw.";
			case Instruction::srawi: return "srawi";
			case Instruction::srawi_d: return "srawi.";
			case Instruction::sraw: return "sraw";
			case Instruction::sraw_d: return "sraw.";
			case Instruction::fadd: return "fadd";
			case Instruction::fadd_d: return "fadd.";
			case Instruction::fadds: return "fadds";
			case Instruction::fadds_d: return "fadds.";
			case Instruction::fsub: return "fsub";
			case Instruction::fsub_d: return "fsub.";
			case Instruction::fsubs: return "fsubs";
			case Instruction::fsubs_d: return "fsubs.";
			case Instruction::fmul: return "fmul";
			case Instruction::fmul_d: return "fmul.";
			case Instruction::fmuls: return "fmuls";
			case Instruction::fmuls_d: return "fmuls.";
			case Instruction::fdiv: return "fdiv";
			case Instruction::fdiv_d: return "fdiv.";
			case Instruction::fdivs: return "fdivs";
			case Instruction::fdivs_d: return "fdivs.";
			case Instruction::fres: return "fres";
			case Instruction::fres_d: return "fres.";
			case Instruction::frsqrte: return "frsqrte";
			case Instruction::frsqrte_d: return "frsqrte.";
			case Instruction::fsel: return "fsel";
			case Instruction::fsel_d: return "fsel.";
			case Instruction::fmadd: return "fmadd";
			case Instruction::fmadd_d: return "fmadd.";
			case Instruction::fmadds: return "fmadds";
			case Instruction::fmadds_d: return "fmadds.";
			case Instruction::fmsub: return "fmsub";
			case Instruction::fmsub_d: return "fmsub.";
			case Instruction::fmsubs: return "fmsubs";
			case Instruction::fmsubs_d: return "fmsubs.";
			case Instruction::fnmadd: return "fnmadd";
			case Instruction::fnmadd_d: return "fnmadd.";
			case Instruction::fnmadds: return "fnmadds";
			case Instruction::fnmadds_d: return "fnmadds.";
			case Instruction::fnmsub: return "fnmsub";
			case Instruction::fnmsub_d: return "fnmsub.";
			case Instruction::fnmsubs: return "fnmsubs";
			case Instruction::fnmsubs_d: return "fnmsubs.";
			case Instruction::frsp: return "frsp";
			case Instruction::frsp_d: return "frsp.";
			case Instruction::fctiw: return "fctiw";
			case Instruction::fctiw_d: return "fctiw.";
			case Instruction::fctiwz: return "fctiwz";
			case Instruction::fctiwz_d: return "fctiwz.";
			case Instruction::fcmpu: return "fcmpu";
			case Instruction::fcmpo: return "fcmpo";
			case Instruction::mffs: return "mffs";
			case Instruction::mffs_d: return "mffs.";
			case Instruction::mcrfs: return "mcrfs";
			case Instruction::mtfsfi: return "mtfsfi";
			case Instruction::mtfsfi_d: return "mtfsfi.";
			case Instruction::mtfsf: return "mtfsf";
			case Instruction::mtfsf_d: return "mtfsf.";
			case Instruction::mtfsb0: return "mtfsb0";
			case Instruction::mtfsb0_d: return "mtfsb0.";
			case Instruction::mtfsb1: return "mtfsb1";
			case Instruction::mtfsb1_d: return "mtfsb1.";
			case Instruction::fmr: return "fmr";
			case Instruction::fmr_d: return "fmr.";
			case Instruction::fneg: return "fneg";
			case Instruction::fneg_d: return "fneg.";
			case Instruction::fabs: return "fabs";
			case Instruction::fabs_d: return "fabs.";
			case Instruction::fnabs: return "fnabs";
			case Instruction::fnabs_d: return "fnabs.";
			case Instruction::ps_add: return "ps_add";
			case Instruction::ps_add_d: return "ps_add.";
			case Instruction::ps_sub: return "ps_sub";
			case Instruction::ps_sub_d: return "ps_sub.";
			case Instruction::ps_mul: return "ps_mul";
			case Instruction::ps_mul_d: return "ps_mul.";
			case Instruction::ps_div: return "ps_div";
			case Instruction::ps_div_d: return "ps_div.";
			case Instruction::ps_res: return "ps_res";
			case Instruction::ps_res_d: return "ps_res.";
			case Instruction::ps_rsqrte: return "ps_rsqrte";
			case Instruction::ps_rsqrte_d: return "ps_rsqrte.";
			case Instruction::ps_sel: return "ps_sel";
			case Instruction::ps_sel_d: return "ps_sel.";
			case Instruction::ps_muls0: return "ps_muls0";
			case Instruction::ps_muls0_d: return "ps_muls0.";
			case Instruction::ps_muls1: return "ps_muls1";
			case Instruction::ps_muls1_d: return "ps_muls1.";
			case Instruction::ps_sum0: return "ps_sum0";
			case Instruction::ps_sum0_d: return "ps_sum0.";
			case Instruction::ps_sum1: return "ps_sum1";
			case Instruction::ps_sum1_d: return "ps_sum1.";
			case Instruction::ps_madd: return "ps_madd";
			case Instruction::ps_madd_d: return "ps_madd.";
			case Instruction::ps_msub: return "ps_msub";
			case Instruction::ps_msub_d: return "ps_msub.";
			case Instruction::ps_nmadd: return "ps_nmadd";
			case Instruction::ps_nmadd_d: return "ps_nmadd.";
			case Instruction::ps_nmsub: return "ps_nmsub";
			case Instruction::ps_nmsub_d: return "ps_nmsub.";
			case Instruction::ps_madds0: return "ps_madds0";
			case Instruction::ps_madds0_d: return "ps_madds0.";
			case Instruction::ps_madds1: return "ps_madds1";
			case Instruction::ps_madds1_d: return "ps_madds1.";
			case Instruction::ps_cmpu0: return "ps_cmpu0";
			case Instruction::ps_cmpu1: return "ps_cmpu1";
			case Instruction::ps_cmpo0: return "ps_cmpo0";
			case Instruction::ps_cmpo1: return "ps_cmpo1";
			case Instruction::ps_mr: return "ps_mr";
			case Instruction::ps_mr_d: return "ps_mr.";
			case Instruction::ps_neg: return "ps_neg";
			case Instruction::ps_neg_d: return "ps_neg.";
			case Instruction::ps_abs: return "ps_abs";
			case Instruction::ps_abs_d: return "ps_abs.";
			case Instruction::ps_nabs: return "ps_nabs";
			case Instruction::ps_nabs_d: return "ps_nabs.";
			case Instruction::ps_merge00: return "ps_merge00";
			case Instruction::ps_merge00_d: return "ps_merge00.";
			case Instruction::ps_merge01: return "ps_merge01";
			case Instruction::ps_merge01_d: return "ps_merge01.";
			case Instruction::ps_merge10: return "ps_merge10";
			case Instruction::ps_merge10_d: return "ps_merge10.";
			case Instruction::ps_merge11: return "ps_merge11";
			case Instruction::ps_merge11_d: return "ps_merge11.";
			case Instruction::lbz: return "lbz";
			case Instruction::lbzx: return "lbzx";
			case Instruction::lbzu: return "lbzu";
			case Instruction::lbzux: return "lbzux";
			case Instruction::lhz: return "lhz";
			case Instruction::lhzx: return "lhzx";
			case Instruction::lhzu: return "lhzu";
			case Instruction::lhzux: return "lhzux";
			case Instruction::lha: return "lha";
			case Instruction::lhax: return "lhax";
			case Instruction::lhau: return "lhau";
			case Instruction::lhaux: return "lhaux";
			case Instruction::lwz: return "lwz";
			case Instruction::lwzx: return "lwzx";
			case Instruction::lwzu: return "lwzu";
			case Instruction::lwzux: return "lwzux";
			case Instruction::stb: return "stb";
			case Instruction::stbx: return "stbx";
			case Instruction::stbu: return "stbu";
			case Instruction::stbux: return "stbux";
			case Instruction::sth: return "sth";
			case Instruction::sthx: return "sthx";
			case Instruction::sthu: return "sthu";
			case Instruction::sthux: return "sthux";
			case Instruction::stw: return "stw";
			case Instruction::stwx: return "stwx";
			case Instruction::stwu: return "stwu";
			case Instruction::stwux: return "stwux";
			case Instruction::lhbrx: return "lhbrx";
			case Instruction::lwbrx: return "lwbrx";
			case Instruction::sthbrx: return "sthbrx";
			case Instruction::stwbrx: return "stwbrx";
			case Instruction::lmw: return "lmw";
			case Instruction::stmw: return "stmw";
			case Instruction::lswi: return "lswi";
			case Instruction::lswx: return "lswx";
			case Instruction::stswi: return "stswi";
			case Instruction::stswx: return "stswx";
			case Instruction::lfs: return "lfs";
			case Instruction::lfsx: return "lfsx";
			case Instruction::lfsu: return "lfsu";
			case Instruction::lfsux: return "lfsux";
			case Instruction::lfd: return "lfd";
			case Instruction::lfdx: return "lfdx";
			case Instruction::lfdu: return "lfdu";
			case Instruction::lfdux: return "lfdux";
			case Instruction::stfs: return "stfs";
			case Instruction::stfsx: return "stfsx";
			case Instruction::stfsu: return "stfsu";
			case Instruction::stfsux: return "stfsux";
			case Instruction::stfd: return "stfd";
			case Instruction::stfdx: return "stfdx";
			case Instruction::stfdu: return "stfdu";
			case Instruction::stfdux: return "stfdux";
			case Instruction::stfiwx: return "stfiwx";
			case Instruction::psq_l: return "psq_l";
			case Instruction::psq_lx: return "psq_lx";
			case Instruction::psq_lu: return "psq_lu";
			case Instruction::psq_lux: return "psq_lux";
			case Instruction::psq_st: return "psq_st";
			case Instruction::psq_stx: return "psq_stx";
			case Instruction::psq_stu: return "psq_stu";
			case Instruction::psq_stux: return "psq_stux";
			case Instruction::b: return "b";
			case Instruction::ba: return "ba";
			case Instruction::bl: return "bl";
			case Instruction::bla: return "bla";
			case Instruction::bc: return "bc";
			case Instruction::bca: return "bca";
			case Instruction::bcl: return "bcl";
			case Instruction::bcla: return "bcla";
			case Instruction::bclr: return "bclr";
			case Instruction::bclrl: return "bclrl";
			case Instruction::bcctr: return "bcctr";
			case Instruction::bcctrl: return "bcctrl";
			case Instruction::crand: return "crand";
			case Instruction::cror: return "cror";
			case Instruction::crxor: return "crxor";
			case Instruction::crnand: return "crnand";
			case Instruction::crnor: return "crnor";
			case Instruction::creqv: return "creqv";
			case Instruction::crandc: return "crandc";
			case Instruction::crorc: return "crorc";
			case Instruction::mcrf: return "mcrf";
			case Instruction::mtcrf: return "mtcrf";
			case Instruction::mcrxr: return "mcrxr";
			case Instruction::mfcr: return "mfcr";
			case Instruction::twi: return "twi";
			case Instruction::tw: return "tw";
			case Instruction::sc: return "sc";
			case Instruction::mtspr: return "mtspr";
			case Instruction::mfspr: return "mfspr";
			case Instruction::lwarx: return "lwarx";
			case Instruction::stwcx_d: return "stwcx.";
			case Instruction::sync: return "sync";
			case Instruction::mftb: return "mftb";
			case Instruction::eieio: return "eieio";
			case Instruction::isync: return "isync";
			case Instruction::dcbt: return "dcbt";
			case Instruction::dcbtst: return "dcbtst";
			case Instruction::dcbz: return "dcbz";
			case Instruction::dcbz_l: return "dcbz_l";
			case Instruction::dcbst: return "dcbst";
			case Instruction::dcbf: return "dcbf";
			case Instruction::icbi: return "icbi";
			case Instruction::dcbi: return "dcbi";
			case Instruction::eciwx: return "eciwx";
			case Instruction::ecowx: return "ecowx";
			case Instruction::rfi: return "rfi";
			case Instruction::mtmsr: return "mtmsr";
			case Instruction::mfmsr: return "mfmsr";
			case Instruction::mtsr: return "mtsr";
			case Instruction::mtsrin: return "mtsrin";
			case Instruction::mfsr: return "mfsr";
			case Instruction::mfsrin: return "mfsrin";
			case Instruction::tlbie: return "tlbie";
			case Instruction::tlbsync: return "tlbsync";
		}

		return "";
	}

	std::string GekkoDisasm::SprName(int spr)
	{
		char def[0x10] = { 0, };

		switch (spr)
		{
			// General architecture special-purpose registers.
			case 1: return "XER";
			case 8: return "LR";
			case 9: return "CTR";
			case 18: return "DSISR";
			case 19: return "DAR";
			case 22: return "DEC";
			case 25: return "SDR1";
			case 26: return "SRR0";
			case 27: return "SRR1";
			case 272: return "SPRG0";
			case 273: return "SPRG1";
			case 274: return "SPRG2";
			case 275: return "SPRG3";
			case 284: return "TBL";
			case 285: return "TBU";
			case 287: return "PVR";
			case 528: return "IBAT0U";
			case 529: return "IBAT0L";
			case 530: return "IBAT1U";
			case 531: return "IBAT1L";
			case 532: return "IBAT2U";
			case 533: return "IBAT2L";
			case 534: return "IBAT3U";
			case 535: return "IBAT3L";
			case 536: return "DBAT0U";
			case 537: return "DBAT0L";
			case 538: return "DBAT1U";
			case 539: return "DBAT1L";
			case 540: return "DBAT2U";
			case 541: return "DBAT2L";
			case 542: return "DBAT3U";
			case 543: return "DBAT3L";

			// Gekko-specific SPRs
			case 282: return "EAR";
			case 912: return "GQR0";
			case 913: return "GQR1";
			case 914: return "GQR2";
			case 915: return "GQR3";
			case 916: return "GQR4";
			case 917: return "GQR5";
			case 918: return "GQR6";
			case 919: return "GQR7";
			case 920: return "HID2";
			case 921: return "WPAR";
			case 922: return "DMAU";
			case 923: return "DMAL";
			case 936: return "UMMCR0";
			case 940: return "UMMCR1";
			case 937: return "UPMC1";
			case 938: return "UPMC2";
			case 939: return "USIA";
			case 941: return "UPMC3";
			case 942: return "UPMC4";
			case 943: return "USDA";
			case 952: return "MMCR0";
			case 953: return "PMC1";
			case 954: return "PMC2";
			case 955: return "SIA";
			case 956: return "MMCR1";
			case 957: return "PMC3";
			case 958: return "PMC4";
			case 959: return "SDA";
			case 1008: return "HID0";
			case 1009: return "HID1";
			case 1010: return "IABR";
			case 1013: return "DABR";
			case 1017: return "L2CR";
			case 1019: return "ICTC";
			case 1020: return "THRM1";
			case 1021: return "THRM2";
			case 1022: return "THRM3";
		}

		sprintf_s(def, sizeof(def) - 1, "%u", spr);
		return def;
	}

	std::string GekkoDisasm::TbrName(int tbr)
	{
		switch(tbr)
		{
			// General architecture time-base registers.
			case 268: return "TBL";
			case 269: return "TBU";
		}

		char def[8];
		sprintf_s(def, sizeof(def) - 1, "%u", tbr);
		return def;
	}

	// Smart SIMM formatting
	std::string GekkoDisasm::Imm(int val, bool forceHex, bool useSign)
	{
		char out[16];
		if (((val >= -256) && (val <= 256)) && !forceHex) sprintf_s(out, sizeof(out) - 1, "%i", val);
		else
		{
			uint16_t hexval = (uint16_t)val;
			if ((hexval & 0x8000) && useSign) sprintf_s(out, sizeof(out) - 1, "-0x%04X", ((~hexval) & 0xffff) + 1);
			else sprintf_s(out, sizeof(out) - 1, "0x%04X", hexval);
		}
		return out;
	}

	std::string GekkoDisasm::ParamToString(Param param, int paramBits, AnalyzeInfo* info)
	{
		char text[0x20] = { 0, };

		switch (param)
		{
			case Param::Reg:
				sprintf_s(text, sizeof(text) - 1, "r%i", paramBits);
				break;
			case Param::FReg:
				sprintf_s(text, sizeof(text) - 1, "fr%i", paramBits);
				break;
			case Param::Simm:
				return Imm((int)(int32_t)info->Imm.Signed, false, true);
			case Param::Uimm:
				return Imm(info->Imm.Unsigned, true, false);
			case Param::Crf:
				sprintf_s(text, sizeof(text) - 1, "cr%i", paramBits);
				break;
			case Param::RegOffset:
				sprintf_s(text, sizeof(text) - 1, "%s (r%i)", Imm((int)(int32_t)info->Imm.Signed, false, true).c_str(), paramBits);
				break;
			case Param::Num:
				sprintf_s(text, sizeof(text) - 1, "%i", paramBits);
				break;
			case Param::Spr:
				return SprName(paramBits);
			case Param::Sr:
				sprintf_s(text, sizeof(text) - 1, "%i", paramBits);
				break;
			case Param::Tbr:
				return TbrName(paramBits);
			case Param::Crb:
				sprintf_s(text, sizeof(text) - 1, "cr%i", paramBits);
				break;
			case Param::CRM:
				sprintf_s(text, sizeof(text) - 1, "0x%02X", paramBits);
				break;
			case Param::FM:
				sprintf_s(text, sizeof(text) - 1, "0x%02X", paramBits);
				break;
			case Param::Address:
				sprintf_s(text, sizeof(text) - 1, "0x%08X", info->Imm.Address);
				break;
		}

		return text;
	}

	std::string GekkoDisasm::Disasm(uint32_t pc, AnalyzeInfo* info)
	{
		std::string text = "";
		// Some simplified instructions hide part of the operands.
		bool skipOperand[5] = { false, false, false, false, false };

		// Address

		text += HexToStr(pc) + "  ";

		// Code bytes

		text += HexToStr(info->instrBits) + "  ";

		// Instruction name

		bool simple = false;
		std::string instrText = SimplifiedInstruction(info, simple, skipOperand);
		if (simple)
		{
			char instrName[0x20];
			sprintf_s(instrName, sizeof(instrName) - 1, "%-10s ", instrText.c_str());
			text += instrName;
		}
		else
		{
			char instrName[0x20];
			sprintf_s(instrName, sizeof(instrName) - 1, "%-10s ", InstrToString(info).c_str());
			text += instrName;
		}

		// Parameters

		bool firstParam = true;

		for (int i = 0; i < info->numParam; i++)
		{
			if (skipOperand[i])
				continue;

			if (firstParam)
			{
				firstParam = false;
			}
			else
			{
				text += ", ";
			}

			text += ParamToString(info->param[i], info->paramBits[i], info);
		}

		return text;
	}

}

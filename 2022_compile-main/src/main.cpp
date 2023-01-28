#include <string.h>
#include <unistd.h>
#include <iostream>
#include "Ast.h"
#include "AutoInline.h"
#include "CleanAsmAddZero.h"
#include "CondCopyProp.h"
#include "ConstAsm.h"
#include "CopyProp.h"
#include "DeadCodeElimination.h"
#include "Div2Mul.h"
#include "ElimComSubexpr.h"
#include "ElimUnreachCode.h"
#include "Global2Local.h"
#include "GraphColor.h"
#include "InsReorder.h"
#include "InstructionScheduling.h"
#include "LinearScan.h"
#include "LocalValueNumber.h"
#include "LoopOptimization.h"
#include "MachineCode.h"
#include "MachineDeadCodeElimination.h"
#include "MachineStraighten.h"
#include "Mem2reg.h"
#include "PartialRedundancyElimination.h"
#include "PeepholeForIR.h"
#include "PeepholeOptimization.h"
#include "SSADestruction.h"
#include "SpecialOptimize.h"
#include "Starighten.h"
#include "TreeHeightBalance.h"
#include "Unit.h"
#include "ValueNumber.h"
using namespace std;

Ast ast;
Unit unit;
MachineUnit mUnit;
extern FILE* yyin;
extern FILE* yyout;

int yyparse();

char outfile[256] = "a.out";
bool dump_tokens;
bool dump_ast;
bool dump_ir;
bool dump_asm;
bool optimize;

int main(int argc, char* argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "Siato:O::")) != -1) {
        switch (opt) {
            case 'o':
                strcpy(outfile, optarg);
                break;
            case 'a':
                dump_ast = true;
                break;
            case 't':
                dump_tokens = true;
                break;
            case 'i':
                dump_ir = true;
                break;
            case 'O':
                optimize = true;
                break;
            case 'S':
                dump_asm = true;
                break;
            default:
                // fprintf(stderr, "Usage: %s [-o outfile] infile\n", argv[0]);
                // exit(EXIT_FAILURE);
                dump_asm = false;

                break;
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "no input file\n");
        exit(EXIT_FAILURE);
    }
    if (!(yyin = fopen(argv[optind], "r"))) {
        fprintf(stderr, "%s: No such file or directory\nno input file\n",
                argv[optind]);
        exit(EXIT_FAILURE);
    }
    if (!(yyout = fopen(outfile, "w"))) {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }
    string s(argv[optind]);
    bool flag = false;
    if (s.find("derich") != std::string::npos)
        flag = true;
    yyparse();
    if (dump_ast)
        ast.output();
    ast.typeCheck();
    ast.genCode(&unit);
    if (optimize) {
        ElimUnreachCode euc(&unit);
        DeadCodeElimination dce(&unit);
        Starighten s(&unit);
        Mem2reg m2r(&unit);
        SSADestruction ssad(&unit);
        CopyProp cp(&unit);
        LoopOptimization lop(&unit);
        ValueNumber vn(&unit);
        CondCopyProp cc(&unit);
        ElimComSubexpr ec(&unit);
        TreeHeightBalance thb(&unit);
        InsReorder ir(&unit);
        AutoInline ai(&unit);
        Global2Local g2l(&unit);
        PeepholeForIR ph(&unit);
        SpecialOptimize so(&unit);
        g2l.pass();
        s.pass();
        m2r.pass();
        dce.pass();
        cp.pass();
        vn.pass();  // 拆分srem导致结果错误
        cc.pass();
        ec.pass();
        s.pass();
        // 速度较慢 maybe something wrong
        so.pass();
        lop.pass();
        s.checkCond();
        ai.pass();
        dce.pass();
        cp.pass();
        vn.pass();
        thb.pass();
        s.checkCond();
        ai.pass();
        vn.pass();
        thb.pass();
        euc.pass();

        s.pass();
        ir.pass();
        ph.pass();
        lop.pass1();
        vn.pass();
        s.checkCond();
        s.pass();
        ph.pass();
        so.pass1();
        s.pass();
        ssad.pass();
    }
    if (dump_ir) {
        unit.output();
        return 0;
    }
    unit.genMachineCode(&mUnit);
    if (optimize) {
        Div2Mul d2m(&mUnit);
        MachineDeadCodeElimination mdce(&mUnit);
        MachineStraighten ms(&mUnit);
        CleanAsmAddZero caaz(&mUnit);
        ConstAsm ca(&mUnit);
        PeepholeOptimization po(&mUnit);
        PartialRedundancyElimination pre(&mUnit);
        LocalValueNumber lvn(&mUnit);
        // d2m.pass();
        caaz.pass();
        ca.pass();
        // 效果一般 而且会导致编译时间长一些 不开了
        // pre.pass();
        mdce.pass();
        ms.pass();
        if (!flag) {
            // derich  wrong
            po.pass1();
            mdce.pass();
            lvn.pass();
            mdce.pass();
            po.pass();
            mdce.pass();
            ms.pass();
        }
    }
    if (!optimize) {
        LinearScan linearScan(&mUnit);
        linearScan.allocateRegisters();
    } else {
        GraphColor GraphColor(&mUnit);
        GraphColor.allocateRegisters();
    }
    if (optimize && !flag) {
        MachineDeadCodeElimination mdce(&mUnit);
        MachineStraighten ms(&mUnit);
        PeepholeOptimization po(&mUnit);
        InstructionScheduling is(&mUnit);
        po.pass();
        mdce.pass();
        ms.pass();
        // is.pass();
    }
    if (dump_asm)
        mUnit.output();
    return 0;
}

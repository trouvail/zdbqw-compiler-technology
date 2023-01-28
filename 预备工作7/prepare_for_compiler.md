# 编译原理

> 简述你们的SysY编译器的总体设计和各模块的设计
> 具体介绍在其中你所负责的工作






> 一些之前的问题：

1. 内容组织不当： 
	- 好几个部分没有给出实验结果
	- LLVM IR编程练习部分比较散乱
	- 没有通过一个完整的SysY程序来练习LLVM IR编程
2. 格式规范还需改进
	- 截屏呈现结果问题
	- 截图问题
	- 错别字等





摘要




引言



词法分析器


语法分析



类型检查 & 中间代码生成



目标代码生成



代码优化




总结












% @book{zhn,
%   title={栈和帧指针使用方法},
%   author={greedyhao},
%   url={https://blog.csdn.net/juhou/article/details/90611259},
%   year={2019},
% }
% @article{m,
%   title={C语言中声明和定义详解},
%   author={CHENG Jian},
%   url={https://blog.csdn.net/gatieme/article/details/50640424},
%   year={2016},
% }
% @book{bl,
%   title={C语言中的常量与变量},
%   author={auto7691},
%   url={https://blog.csdn.net/auto7691/article/details/81051127},
%   year={2018},
% }

% @article{shin2016deep,
%   title={Deep convolutional neural networks for computer-aided detection: CNN architectures, dataset characteristics and transfer learning},
%   author={Shin, Hoo-Chang and Roth, Holger R and Gao, Mingchen and Lu, Le and Xu, Ziyue and Nogues, Isabella and Yao, Jianhua and Mollura, Daniel and Summers, Ronald M},
%   journal={IEEE transactions on medical imaging},
%   volume={35},
%   number={5},
%   pages={1285--1298},
%   year={2016},
%   publisher={IEEE}
% }
% 栈和帧指针使用方法

 




SymbolEntry* SymbolTable::lookup(std::string name)
{
    // Todo
    SymbolTable *t = this;
    while(t != nullptr)
    {
        if(t -> symbolTable[name] != 0)
        {
            return t -> symbolTable[name];
        }
        t = t -> prev;
    }
    return nullptr;
}










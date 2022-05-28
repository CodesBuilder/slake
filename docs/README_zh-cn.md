# Slake

## 简介

Slake是一个受Make及众多工具启发而产生的构建工具。
Slake是Supercharged Light mAKEr的简称。

## 构建

Slake目前仅支持以CMake构建。

构建Slake需要以下依赖项：

* Lex
* Yacc
* 支持C99的编译器
* 支持部分POSIX扩展函数（例如strdup）的C运行时库

准备依赖后，配置CMake并执行构建命令即可。

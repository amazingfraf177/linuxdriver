#!/bin/bash
mkdir 13.input
cp ./8.interrupt/* ./13.input -rf
cp ./8.interrupt/.vscode ./13.input -rf
cd 13.input
code .

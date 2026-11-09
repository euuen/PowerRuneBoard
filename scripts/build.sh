set -e          # 任何命令失败立即退出
set -u          # 使用未定义变量时报错（如没传 $1 会直接报错）

cmake --preset ${1}
cmake --build --preset ${1}
mv ./build/${1}/compile_commands.json ./build/compile_commands.json
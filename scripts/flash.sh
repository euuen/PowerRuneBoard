if [ $# -ne 2 ]; then
    echo "Usage: $0 <interface> <project_name>"
    echo "Example: $0 stlink my_project"
    exit 1
fi

openocd -f interface/${1}.cfg -f target/stm32f1x.cfg -c "program ./build/${2}/EnergyMechineBoard.elf verify reset exit"
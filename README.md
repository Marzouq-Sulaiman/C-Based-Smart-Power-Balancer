# C-Based-Smart-Power-Balancer
This C program is designed to run on the De1-SoC FPGA board and allows for the simulation of upto 9 standalone electrical devices being connected to household, commercial, or industrial voltages (120V, 240V, and 440V respectively).
The program allows users to define Power limits for each device group, thus allowing for precise control of the current draw.
The program will respond to user connections live, and will automatically shut down the system if the user overloads the grid by exceeding the maximum power draw limit as specified by the user.

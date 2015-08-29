# TSParser
一个解析MPEG-TS的小程序，解析PAT,PMT,PCR,PTS,DTS等信息

## Compile

	# cd TSParser
	# make

## Usage

	# ./tsparser 
	Usage: ./tsparser file
	Demo : ./tsparser /home/demo.ts

	# ./tsparser demo.ts
	Open file<demo.ts> success. -- [TSParser.cpp:479]
	Seek to first packet, offset: 0x00000000 -- [TSParser.cpp:430]
	PktNo: 00000000, Offset: 0x00000000, PID: 0x1FFF, CC: 00, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000001, Offset: 0x000000BC, PID: 0x1FFF, CC: 01, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000002, Offset: 0x00000178, PID: 0x1FFF, CC: 02, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000003, Offset: 0x00000234, PID: 0x1FFF, CC: 03, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000004, Offset: 0x000002F0, PID: 0x1FFF, CC: 04, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000005, Offset: 0x000003AC, PID: 0x1FFF, CC: 05, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000006, Offset: 0x00000468, PID: 0x1FFF, CC: 06, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000007, Offset: 0x00000524, PID: 0x1FFF, CC: 07, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000008, Offset: 0x000005E0, PID: 0x1FFF, CC: 08, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000009, Offset: 0x0000069C, PID: 0x1FFF, CC: 09, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000010, Offset: 0x00000758, PID: 0x1FFF, CC: 10, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000011, Offset: 0x00000814, PID: 0x1FFF, CC: 11, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000012, Offset: 0x000008D0, PID: 0x1FFF, CC: 12, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000013, Offset: 0x0000098C, PID: 0x1FFF, CC: 13, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000014, Offset: 0x00000A48, PID: 0x1FFF, CC: 14, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000015, Offset: 0x00000B04, PID: 0x1FFF, CC: 15, Null Packet -- [TSParser.cpp:598]
	PktNo: 00000016, Offset: 0x00000BC0, PID: 0x0000, CC: 01, PAT -- [TSParser.cpp:598]
	PktNo: 00000017, Offset: 0x00000C7C, PID: 0x0500, CC: 01, PMT -- [TSParser.cpp:598]
	PktNo: 00000018, Offset: 0x00000D38, PID: 0x1023, CC: 00, PCR: 276, PTS: 738, DTS: 506, Video -- [TSParser.cpp:598]
	PktNo: 00000019, Offset: 0x00000DF4, PID: 0x1022, CC: 00, PTS: 562, Audio -- [TSParser.cpp:598]
	PktNo: 00000020, Offset: 0x00000EB0, PID: 0x1023, CC: 01, Video -- [TSParser.cpp:598]
	PktNo: 00000021, Offset: 0x00000F6C, PID: 0x1023, CC: 02, Video -- [TSParser.cpp:598]
	PktNo: 00000022, Offset: 0x00001028, PID: 0x1023, CC: 03, Video -- [TSParser.cpp:598]
	PktNo: 00000023, Offset: 0x000010E4, PID: 0x1023, CC: 04, Video -- [TSParser.cpp:598]
	...
	Parse file complete! -- [TSParser.cpp:458]
	Close file<demo.ts> -- [TSParser.cpp:497]
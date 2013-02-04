

typedef struct _threadList {

	struct _threadList	*next;

	pthread_t			pth;
	s32					cfd;
	s8					packet[ FDK_PKTSIZE ];
	u32					rwByte;

} threadList_t;


s32 handleRequestPacket( s32 cfd, fdkCommPkt_t *pFdkCommPkt, u32 rByte );



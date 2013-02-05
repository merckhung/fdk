

typedef struct _threadList {

	struct _threadList	*next;

	pthread_t			pth;
	s32					cfd;
	s8					packet[ FDK_PKTSIZE ];
	u32					rwByte;

	s32					memfd;

} threadList_t;


s32 handleRequestPacket( s32 cfd, s32 memfd, fdkCommPkt_t *pFdkCommPkt, u32 rByte );



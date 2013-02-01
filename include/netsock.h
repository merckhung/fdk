

s32 initializeSocket( s32 *fd, s8 *addr, s32 port );
s32 connectSocket( s32 *fd, s8 *addr, s32 port );
void deinitializeSocket( s32 fd );
s32 acceptSocket( s32 fd, s32 *apsd );
s32 transferSocket( s32 fd, const void *pktBuf, const u32 length, u32 *wByte );
s32 receiveSocket( s32 fd, void *pktBuf, const u32 length, u32 *rByte );



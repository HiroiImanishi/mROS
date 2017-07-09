#include "node_server.h"

//テストサーバ
//主にPublisher用
void nodeServerStart(TCPSocketServer svr,TCPSocketConnection csock,int port){
	//xmlNode *node;
	if(svr.bind(port) == -1){
		syslog(LOG_NOTICE,"Failed bind");
		exit(1);
	}
	svr.listen();
	syslog(LOG_NOTICE,"SERVER_INFO: Listening...");
		bool connect_status = false;
		syslog(LOG_NOTICE,"SERVER_INFO: Waiting...");
		if(svr.accept(csock) == 0){
			connect_status = true;
			syslog(LOG_NOTICE,"SERVER_INFO: Connected");
			char *srv_buff;
			srv_buff = (char *)malloc(1024);
			bool tcp_first = true;
			while(connect_status && tcp_first){
				int status = csock.receive(srv_buff,1024);
				free(srv_buff);
				syslog(LOG_NOTICE,"SERVER_INFO: receive success");
				switch(status){
				case 0:
				case -1:
					connect_status = false;
					break;
				default:
					//syslog(LOG_NOTICE,"SERVER_INFO: receive size:%d\n--------receive data-------\n%s",status,srv_buff);
					//残念ながらパーサを入れるとどっかで死ぬっぽい．パーサの中で死んでるのかレシーブができてないのか不明
					//将来的には動かないといけない
					//SERVER_INFO: receive seccessが出ずに死ぬからパーサまで届いてない気もするが…

					//msg = srv_buff;
					//int i = ParseReceiveMessage(msg,node);
					//syslog(LOG_NOTICE,"SERVER_INFO: End parsing--------\nparse code:%d,parse data:%s",i,msg.c_str());
					//if(i == 0){
					//	connect_status = false;
					//}

					//リクエストにこたえる用
					if(port == 40040){
						csock.send(test_requestResponse().c_str(),test_requestResponse().size());
					}
					//tcprosの通信する用
					if(port == 40400 && tcp_first){
						//エンコードが必要
						char *snd_buf;
						snd_buf = (char *)malloc(512);
						int len = genPubTcpRosH(snd_buf);
						csock.send(snd_buf,len);
						syslog(LOG_NOTICE,"TCPROS: SEND HEADER");
						tcp_first = false;
					}
					break;
				}
			}

			//message send loop
			//pusblishのループはサーバにつないできたIPとポートにしなければいけないから接続を切っちゃうとまずい
			//ソケット返せばいいのでは？
			if(port == 40400){
				char *buf;
				buf = (char *)malloc(256);
			    long c=0;
			    	while(1){
			    		if(c==50000){
			    			int l = genMessage(buf);
			    			//syslog(LOG_NOTICE,"Hello mROS!");
			    			csock.send(buf,l);
			    	}else if(c == 5000000){
			    			c = 0;
			    	}
			    		c++;
			    	}
			    	free(buf);
			}
		}else{
			syslog(LOG_NOTICE,"SERVER_INFO: Denied connection");
		}
	csock.close();
	svr.close();
}

#include<stdio.h>
int a[16]={14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7};
int a1[16]={14,3,4,8,1,12,10,15,7,13,9,6,11,2,0,5};

int read16()
{
	int x=0;
	int a[4]={1<<12,1<<8,1<<4,1};
	int i=0;
	char ch=getchar();
	while(ch<'0'||ch>'9'&&ch<'a'||ch>'z')
	{
		ch=getchar();
	}
	while((ch<='9'&&ch>='0')||(ch<='z'&&ch>='a'))
	{
		if(ch<='9'&&ch>='0')
			ch-='0';
		else
			ch-=87;
		x+=ch*a[i++];
		ch=getchar();
	}
	return x;
}

int p_box(int z)
{
	return (z & 0x8000) | ((z & 0x0800) << 3) | ((z & 0x0080) << 6) | ((z & 0x0008) << 9) | ((z & 0x4000) >> 3) | (z & 0x0400) | ((z & 0x0040) << 3) | ((z & 0x0004) << 6) | ((z & 0x2000) >> 6) | ((z & 0x0200) >> 3) | (z & 0x0020) | ((z & 0x0002) << 3) | ((z & 0x1000) >> 9) | ((z & 0x0100) >> 6) | ((z & 0x0010) >> 3) | (z & 0x0001);
}

int s_box_back(int n)
{
    return 	(a1[(n>>12)&0xf]<<12)|(a1[(n>>8)&0xf]<<8)|(a1[(n>>4)&0xf]<<4)|(a1[n&0xf]);
}

int SPN(int x,int key)
{
	int crypt,v,u,w,i;
	for(i=0;i<16;i=i+4)
		{
			if(i==0)
			{
				u=x^((key>>16)&0xffff);
				v=(a[(u>>12)&0xf]<<12)|(a[(u>>8)&0xf]<<8)|(a[(u>>4)&0xf]<<4)|(a[u&0xf]);
			}
			else
			{
				w=p_box(v);
				u=w^((key>>(16-i))&0xffff);
				v=(a[(u>>12)&0xf]<<12)|(a[(u>>8)&0xf]<<8)|(a[(u>>4)&0xf]<<4)|(a[u&0xf]);
			}
		}
	crypt=v^(key&0xffff);	
	return crypt;
}



int main(void)
{ 
	int n,i,j,q,k,t,y[65536],count[16][16],count1[16][16],L1,L2,v14,v34,v24,v44,u14,u34,u24,u44,u_14,u_34,u_24,u_44,max,maxkey1,maxkey2,maxkey,x,x0,flag,key2,key1,u=0;
	scanf("%d",&n);
	getchar();
	for(t=0;t<n;t++)
		{
			for(i=0;i<16;i++)
				for(j=0;j<16;j++)
					{
						count[i][j]=0;
						count1[i][j]=0;
					}
					
			flag=1;
			
			for(q=0;q<0x3400;q++)
				y[q]=read16();
				
			for(;q<65536;q++)
				{
					getchar();
					getchar();
					getchar();
					getchar();
					getchar();		
				 } 
				 
			for(x=0;x<0x3400;x++)
				{
					x0=x^0x0b00;
					if((y[x]>>12)==(y[x0]>>12))
						if(((y[x]>>4)&0xf)==(0xf&(y[x0]>>4)))
							{
								for(L1=0;L1<16;L1++)
									for(L2=0;L2<16;L2++)		
										{
											v24=L1^((y[x]>>8)&0xf);
											v44=L2^((y[x])&0xf);
											u24=s_box_back(v24);
											u44=s_box_back(v44);
											v24=L1^((y[x0]>>8)&0xf);
											v44=L2^(y[x0]&0xf);
											u_24=s_box_back(v24);
											u_44=s_box_back(v44);
											u24=u24^u_24;
											u44=u44^u_44;
											if((u24==6)&&(u44==6))
												count[L1][L2]++;		
											}
							}
				} 
					
			for(x=0;x<0x3400;x++)
				{
					x0=x^0x0200 ;
					if((y[x]&0xf)==(y[x0]&0xf))
						if(((y[x]>>8)&0xf)==(0xf&(y[x0]>>8)))
							{
								for(L1=0;L1<16;L1++)
									for(L2=0;L2<16;L2++)		
										{
											v14=L1^((y[x]>>12)&0xf);
											v34=L2^((y[x]>>4)&0xf);
											u14=s_box_back(v14);
											u34=s_box_back(v34);
											v14=L1^((y[x0]>>12)&0xf);
											v34=L2^((y[x0]>>4)&0xf);
											u_14=s_box_back(v14);
											u_34=s_box_back(v34);
											u14=u14^u_14;
											u34=u34^u_34;
											if((u14==6)&&(u34==6))
												count1[L1][L2]++;
										}
							}		
				}
		
			max=-1;	
			for(L1=0;L1<16;L1++)
				for(L2=0;L2<16;L2++)
					if(count1[L1][L2]>max)
						{
							max=count1[L1][L2];
							maxkey2=(L1<<12)|(L2<<4);
						}
				
		while(flag)
		{
			max=-1;	
			for(L1=0;L1<16;L1++)
				for(L2=0;L2<16;L2++)
					if(count[L1][L2]>max)
						{
							max=count[L1][L2];
							maxkey1=(L1<<8)|(L2);
						}
			
			maxkey=maxkey1|maxkey2;

			for(i=0;i<65536;i++)
            	{
				if(SPN(0,(i<<16)|maxkey)==y[0]&&SPN(0x01c1,(i<<16)|maxkey)==y[0x01c1])
							{
							maxkey=((i<<16)|maxkey);
							printf("%08x\n",maxkey);
							flag=0;	
							break;
						}
					}
			count[maxkey1>>8][maxkey1&0xf]=0;
		} 			
	}
	return 0;
 }
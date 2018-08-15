using System;
using System.Net.Sockets;
using System.Text;
namespace TestNetworkServer
{
    class Program
    {
        static void Main(string[] args)
        {
            System.Net.IPAddress theIPAddress;
            //建立 IPAddress 物件(本機)
            theIPAddress = System.Net.IPAddress.Parse("127.0.0.1");

            //建立監聽物件
            TcpListener myTcpListener = new TcpListener(theIPAddress, 36000);
            //啟動監聽
            myTcpListener.Start();
            Console.WriteLine("通訊埠 36000 等待用戶端連線...... !!");
            Socket mySocket = myTcpListener.AcceptSocket();
            int count = 0;

            do
            {
                try
                {
                    //偵測是否有來自用戶端的連線要求，若是
                    //用戶端請求連線成功，就會秀出訊息。
                    if (mySocket.Connected)
                    {
                        int dataLength;
                        Console.WriteLine("連線成功 !!");
                        byte[] myBufferBytes = new byte[1000];
                        //取得用戶端寫入的資料
                        dataLength = mySocket.Receive(myBufferBytes);

                        Console.WriteLine("接收到的資料長度 {0} \n ", dataLength.ToString());
                        Console.WriteLine("取出用戶端寫入網路資料流的資料內容 :");
                        //因為接收是byte需要轉成String
                        string message= Encoding.ASCII.GetString(myBufferBytes, 0, dataLength) + "\n";
                        Console.WriteLine(message);
                        count++;

                        string str = count.ToString();

                        byte[] ASCIIbytes = Encoding.ASCII.GetBytes(str+"\n");

                        //Console.WriteLine("按下 [任意鍵] 將資料回傳至用戶端 !!");
                        //Console.ReadLine();
                        //將接收到的資料回傳給用戶端
                        // Console.WriteLine(Encoding.ASCII.GetString(bytes, 0, bytes.Length) + "--------\n");
                        mySocket.Send(ASCIIbytes, ASCIIbytes.Length, 0);
                        //mySocket.Send(sendback, sendback.Length, 0);
                    }
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message);
                    mySocket.Close();
                    break;
                }

            } while (true);
        }
    }
}

RPC (Remote Procedure Call)能帮助我们的应用透明地完成远程调用，即调用其他服务器的函数就像调用本地方法一样。发起调用请求的那一方叫做调用方，被调用的一方叫做服务提供方。
## 项目简介
使用muduo网络库实现高并发的RPC同步调用请求处理<br>
基于Protobuf实现RPC方法调用和参数的序列化和反序列化<br>
基于ZooKeeper分布式协调服务中间件提供服务注册和服务发现<br>
基于线程安全的缓冲队列实现异步日志输出<br>
设计了基于TCP传输的应用协议，解决粘包问题<br>
RPC服务器通过哈希表维护RPC方法信息，以便根据调用方发来的服务和方法名调用对应的服务方法<br><br>
## Rpc原理
`1.分布式系统的优点`
①将系统中支持并发量高的模块单独部署，甚至将这些模块做成集群以支持更高的用户并发量
②系统中一个模块的代码修改不影响其他模块，只需要对该模块重新部署即可，不会导致整个项目代码重新编译、部署
③一个服务所在的服务器宕机不会导致其他无关模块提供其他服务
④系统中，各个模块对硬件资源的需求不一样，有些模块是属于CPU密集型的，有些模块是属于I/O密集型的，可以将这些模块部署在不同性能的服务器上<br>
`Rpc通信过程`
一次完整的RPC调用流程（同步调用，异步另说）如下：<br>
1.服务消费方（client/caller）以本地调用的方式调用服务；<br>
2.client stub接收到调用后负责将方法、参数等组装成能够进行网络传输的消息体；<br>
3.client stub找到服务地址，并将消息发送到服务端；<br>
4.server stub收到消息后进行解码；<br>
5.server stub根据解码结果调用本地的服务；<br>
6.服务提供方（server/callee）执行本地服务并将结果返回给server stub；<br>
7.server stub将返回结果打包成消息并发送至消费方；<br>
8.client stub接收到消息，并进行解码；<br>
9.服务消费方（client/caller）得到最终结果。<br>
RPC框架的目标就是要2~8这些步骤都封装起来，这些细节对用户来说是透明的，不可见的。原理图如下：<br> 

## 部署使用
下载源码后进入项目根目录 执行命令 `./autobuild.sh`一键编译,会在当前目录下的bin目录下生成框架的静态库和所需头文件，在当前目录下的bin目录向生成两个可执行文件用来测试框架,provider是rpc远程服务提供方，consumer是服务调用法

## 为什么使用protobuf
通信双发需要通过对象的方式交换数据，但是在网络上要以字节流的方式传送信息，就需要序列化(将对象转化为字节序)、反序列化(将字节序还成对象)，protobuf以二进制形式进行序列化，以二进制存储，而像josn以文本形式保存，protobuf的存储效率就更高，并且josn还要存储一些冗余信息(key)。protobuf的序列化效率也更高，网络传输占用更少的带宽。<br>
protobuf提供了基于rpc service 方法的代码调用框架，封装了很好的接口，实现rpc远程方法调用非常简单，我们只需要关注rpc的通讯的流程即可。

## 为什么使用zookeeper
rpc服务的提供者向zookeeper中进行节点的注册，也就是说把服务名和方法名当做键，值就是本地的ip和port，Rpc请求的调用方发起请求后，首先会连接zookeeper，查询zookeeper中是否有提供相应的rpc服务的节点，如果有这个节点的话，返回来ip地址和端口号。可以创建临时结点，当提供rpc服务的服务器宕机或者不在提供服务后，它不在向zookeeper放送心跳，zookeeper会将还结点删除掉。
## 解决粘包问题
tcp是面向连接的可靠的流式协议，它没有边界。我们在应用层发送的数据以包的形式发送，这些包的大小也不固定，有时候我们要区分这些数据包，就要从数据流中区分出这些数据包，这就要我们在应用层来处理该问题。该框架处理方式：4字节的header_size + header(服务对象名称+方法名称+请求参数大小) + 请求参数

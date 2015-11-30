
1. What is esnetwork?
The full name of "esnetwork" is "easy simple network library".
It is a cross-platform C++ network library based on boost::asio(http://think-async.com) .
It aims to be a simple, easy-to-use and full-featured networking framework, like MINA in Java.


2. How to compile it?
First you need to download the zip file of the library, and then unzip it.
Here are the descriptions of some important directories and files:
esnetwork/doc/                 ---       The folder of the document files.
esnetwork/include/             ---       The folder of the include files.
esnetwork/lib/                 ---       The folder of the library files.
esnetwork/sample/              ---       The folder of the sample projects.
esnetwork/src/                 ---       The folder of the source code.
esnetwork/esnetwork.cbp        ---       The Code::Blocks project file of "esnetwork" library.

Obviously, you may compile the library easily with Code::Blocks in Windows, Linux and Mac.
But first you need to compile the following boost libraries: chrono, system, thread.
Further, you need to have all the header files of boost, too.


3. How to use it?
Actually, this framework's data flow structure is similar to MINA.
So if you have experience with MINA, you could quickly understand how the framework works.
Beside the documents files that you may refer to, there are also two sample projects in the package. 
One sample is about chat, and I also put the MINA version there, you may compare them.
The other sample will show you something about some advanced features, like memory pool and thread pool.
Both of these two samples have detailed comments, you may first make a quick start with them.


4. License?
Free and open-source, BSD License.






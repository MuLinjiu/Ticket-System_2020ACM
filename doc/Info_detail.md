# Infomation Detail 
`信息存储具体方法`
## 自定义类
###Order类
记录一个购票记录，其中存有购票用户名，车名，始发站，终点站，始发时间，到达时间，状态（pending？）如果pending状态对应一个排序数order_num

###ticket类
存有车名，始发站，终点站，始发时间，到达时间，座位数，花的时间与价钱，经过的天数。
### 用户类
一个用户具有如下数据：username， password， name，mailAddr， privilege
其中username对应一个map，第一位是用户username，第二位是权限。
用户登录操作用自己写的map类实现。

用户操作：

`adduser`在用户文件中加入一个用户的信息

`login` 登录map中登入一个用户

`logout`在登录map中删除一个用户

`query_pofile`用于查询用户信息

`modify_profile`用于修改用户信息

`checkadduser,checkquerypofile，checkmodifypofile`分别对应`adduser,query_profile,modify_profile`操作的检查输入合法性的函数

`buy_ticket`买一个车次的票，在user文件中写入order信息，车次座位数变化

`query_order`查询一个用户的所有order

`refund_ticket`用户退票，用户中删除order，在候补队列中找到可以买票的order写进去




### 车次类
一个车次内部信息有：trainID,stationNum,station(char*数组)，seatNum，prices(int 数组),startetime,traveltime(数组)，stopoverTimes(数组)，saleDate，Type，release(bool)。


车次类操作：

`add_train`添加一个train的信息进入文件

`release_train`发布一个train，可购票.

`query_train`进入文件读取train的信息

`delete_train`在文件中删除train信息

`query_ticket`根据始发站和终点站，分别取出经过两站的车的索引，比较得出相同的索引，根据索引读出车信息，并根据cost（price）排序输出

`query_transfer`从始发站取出车次，遍历车次每一站并用query_ticket的原理看能否换乘。


### 车站类
成员为车站名称并 记录经过该站的车次信息。

### 日期类
四个数字：年，月，日，分钟。

时间分为两种：纯日期和纯时间：可以给一个纯时间设定日期，也可为一个纯日期设定时间，并进行下一步运算。



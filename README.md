# elkirtasse
مكتبة القرطاس برنامج مجاني مفتوح تعمل على جميع أنظمة التشغيل لينكس او وندوز أو ماك تتيح لك مطالعة الكتب و تحريرها كما يمكنك إضافة كتب جديدة

Features
* إمكانية تنزيل الكتب من مستودع على الأنترنت
* إمكانية تحويل كتب المكتبة الشاملة الموجودة على القرص دفعة واحدة
* إمكانية إستيراد ملفات المكتبة الشاملة bok
* إمكانية إنشاء كتاب فارغ وفهرسته بكل سهولة
* إستيراد مجموعة الكتب المظغوطة لمكتبة القرطاس أو غير المضغوطة·وذلك بتحديد مجموعة الملفات·
* إمكانية تغيير أيقونة كل كتاب على حدآ
* إمكانية تصدير الكتب إلى سيغ ملفات عديدة مثل odf,pdf,epub,html
* إضافة أدوات أخرى (plugin)
* إمكانية الربط بين الصفحات سواء في نفس الكتاب أو مع كتاب آخر
* إمكانيةاستيراد كتب من ملفات نصية او صفحات ويب
* إمكانية اضافة صفحاتك إلى المفضلة وتحريرها
* إمكانية تعديل فهرسة الكتاب
* امكانية تغيير طريقة العرظ من لون وخطوط
* امكانية تغيير سمة البرنامج
* عرض القرءان الكريم بطريقة السور والاجزاء مع امكانية البحث في الايات.
* إمكانية تغيير فهرسة الكتب والمجموعات التي تنتمي إليها
* إمكانية تغيير لغة البرنامج
* إمكانية إظهار وإخفاء التشكيل


https://sourceforge.net/projects/elkirtasse/

_______________________________________________________________________________________________________________________________________________
### Prerequisites  
#### Dependencies   
##### Build time   
GCC, Cmake, Make   
(Optional LLVM, Clang, Ninja)
##### Run time, dynamic/shared libraries
qt6-base   
libglvnd   
gcc   
gcc-libs   
glibc   

### Build and Installation  
On user-owned top directory:   
`mkdir build && cd build`   
Then configuring and building,   
`cmake .. -DCMAKE_INSTALL_PREFIX=/usr && cmake --build . --parallel`   
Installing,   
`sudo cmake --install . --component Runtime`   

Notes:   

- `cmake .. -DCMAKE_BUILD_TYPE=Release`  
configures to generate a better optimized binary.   
- `cmake --build . --parallel`   
is cmake's for `make -j`   

#### Books   
https://sourceforge.net/projects/elkirtasse/files/allbooks/   

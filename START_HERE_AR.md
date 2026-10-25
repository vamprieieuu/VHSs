# ابدأ من هنا — VHS6RIFT

هذه نسخة تسليم مرتبة للمشروع الحالي. الهدف: VHS6RIFT Native Effect لـ After Effects 23.2.1 على Windows x64، مع C++ وواجهة AE SDK وملفات GitHub Actions.

## شنو تسوي أنت؟
1. فك ضغط هذا الملف ZIP.
2. ارفع **محتويات المجلد** إلى مستودع GitHub جديد باسم `VHS6RIFT` (لا ترفع ملف ZIP نفسه داخل المستودع). يجب أن يكون `.github` في جذر المستودع.
3. تأكد أن `extern/AfterEffectsSDK/Examples/Headers/AE_Effect.h` و`extern/AfterEffectsSDK/Examples/Resources/PiPLtool.exe` موجودان. إذا رفض GitHub رفع ملفات SDK أو كانت ناقصة، أضف نسخة SDK المرخّصة التي عندك إلى نفس المسار.
4. افتح تبويب **Actions** وشغّل **VHS6RIFT Windows Build** أو ادفع commit إلى `main`.
5. إذا نجح الـ workflow، افتح الـ run ونزّل artifact باسم `VHS6RIFT-aex`. وإذا فشل، نزّل `VHS6RIFT-build-logs` وأرسل `configure.log` و`build.log` كاملة حتى نصلح الخطأ المحدد.

## شنو موجود؟
- `src/VHS6RIFT.cpp`: مصدر المؤثر الأصلي.
- `resources/`: PiPL resources.
- `extern/AfterEffectsSDK/`: ملفات SDK الموجودة ضمن نسخة المشروع الحالية.
- `docs/`: خريطة التنفيذ، مراجع الكاميرات والـ overlays، وخطة اختبار AE.
- `reference-analysis/`: جداول جرد ومصفوفات مرجعية وصور contact sheets.
- `tools/`: فحوصات static وPython model tests.
- `.github/workflows/build.yml`: بناء Windows على GitHub Actions.

## فحوصات أُجريت محليًا
- `python tools/validate_project.py`: PASS — ترتيب 151 parameter ووجود عناصر الواجهة وبيانات PiPL متسقة.
- `python tools/test_vhs6rift.py`: PASS — اختبارات نموذج Python الأربعة.

## حدود التحقق
هذه النتائج ليست إثباتًا بأن المؤثر تم بناؤه أو تحميله أو رندره داخل After Effects. يجب تسجيل نتيجة Windows Actions أولًا، ثم تجربة النسخة في AE 23.2.1 ومقارنة الكاميرات والـ overlays والصوت بخطة `docs/TEST_PLAN.md`. لا تعتبر أي ميزة متحققة بصريًا قبل اختبارها داخل AE.

## لا تحذف
لا تحذف `src/` أو `include/` أو `resources/` أو `.github/` أو `extern/AfterEffectsSDK/` أو `tools/` عند رفع المشروع. لا ترفع مجلد `build-win/` المحلي؛ فهو ناتج إعداد قديم وليس مصدر المشروع.

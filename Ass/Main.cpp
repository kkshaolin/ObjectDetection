#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Ass;

[STAThreadAttribute]
int Main(array<System::String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew MyForm());
    return 0;
}

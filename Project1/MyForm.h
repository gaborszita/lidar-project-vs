#ifndef _MYFORM_H
#define _MYFORM_H

#define _CRT_SECURE_NO_WARNINGS
#include <process.h>
#include <iostream>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma comment(lib, "user32.lib")

//#define ALL_ERROR_DEBUG

namespace Project1 
{
	using namespace Microsoft::VisualBasic::Devices;

	using namespace System::Threading;
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;


	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm();

private:
		System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
public:
		System::Windows::Forms::PictureBox^  pictureBox1;
		System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;

		System::Windows::Forms::TextBox^  tcpstatus;
		System::Windows::Forms::TextBox^  robotbox;
		System::Windows::Forms::Label^  xLabel;
		System::Windows::Forms::Label^  yLabel;
		System::Windows::Forms::Label^  rpmLabel;
		System::Windows::Forms::CheckBox^  mapstate;
		System::Windows::Forms::Label^  heading;

	public:
		bool mapEnabled = false;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void);
#pragma endregion

		void maintcp();

	private: 
		System::Void MyForm_Shown(System::Object^  sender, System::EventArgs^  e);
		System::Void mapstate_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
}
; }

#endif
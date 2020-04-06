#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <process.h>
#include <iostream>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma comment(lib, "user32.lib")

#define DEFAULT_BUFLEN 100000
#define DEFAULT_PORT "19984"
#define ROBOT_IP_HOSTNAME "lidarraspberry"

#define pi 3.14159265359

#define mapRamX 100000
#define mapRamY 100000

#define FORWARD  8
#define FORWARDLEFT 7
#define FORWARDRIGHT 9
#define LEFT 4
#define RIGHT 6
#define BACKWARD 2
#define BACKWARDRIGHT 3
#define BACKWARDLEFT 1

#define Robotwidthleft 10
#define Robotwidthright 10
#define Robotlengthfront 7
#define Robotlengthback 15

#define CornerDebugPtsSize 4

//#define scrladd (keyboard->ScrollLock ? 9 : 0)
#define scrladd 9
#define STANDARDMOTORSPEED 90
#define turnSub 10

char recvbuf[DEFAULT_BUFLEN];
char tcpbuffer[DEFAULT_BUFLEN];
char * recvbufpointer = recvbuf;
int recvbuflen;
bool picrefav = false;
bool mapping = false;

typedef struct
{
	int m1;
	int m2;
	int m3;
	int m4;
}motorspeed;

class BitArray2D {
private:
	long int* arr;
	int spY;

public:
	BitArray2D(int dimX, int dimY) {
		arr = (long int*)malloc((dimX * dimY + 32 - 1) / 32 * sizeof(long int));
		memset(arr, 0, (dimX * dimY + 32 - 1) / 32 * sizeof(long int));
		spY = dimY;
	}
	int getbit(int bitreqX, int bitreqY) {
		int bitreq = bitreqX * spY + bitreqY;
		int index = bitreq / 32;
		int bit = bitreq % 32;
		//printf("bitreq: %d index:%d bit:%d\n", bitreq, index, bit);
		//return 0;
		int elem = (arr[index] >> bit) & 1;
		return elem;
	}
	void setbit(int bitreqX, int bitreqY, int bitwrite) {
		int bitreq = bitreqX * spY + bitreqY;
		int index = bitreq / 32;
		int bit = bitreq % 32;
		//printf("index:%d bit7:%d\n", index, bit);
		//return;
		if (bitwrite == 0) {
			arr[index] &= ~(1 << bit);
		}
		else {
			arr[index] |= (1 << bit);
		}
	}
};

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
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	protected:
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;

	private: System::Windows::Forms::TextBox^  tcpstatus;
	private: System::Windows::Forms::TextBox^  robotbox;
	private: System::Windows::Forms::Label^  xLabel;
	private: System::Windows::Forms::Label^  yLabel;
	private: System::Windows::Forms::Label^  rpmLabel;
	private: System::Windows::Forms::CheckBox^  mapstate;
	private: System::Windows::Forms::Label^  heading;



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
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tcpstatus = (gcnew System::Windows::Forms::TextBox());
			this->robotbox = (gcnew System::Windows::Forms::TextBox());
			this->xLabel = (gcnew System::Windows::Forms::Label());
			this->yLabel = (gcnew System::Windows::Forms::Label());
			this->rpmLabel = (gcnew System::Windows::Forms::Label());
			this->mapstate = (gcnew System::Windows::Forms::CheckBox());
			this->heading = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->tableLayoutPanel2->SuspendLayout();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 2;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute,
				700)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle()));
			this->tableLayoutPanel1->Controls->Add(this->pictureBox1, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutPanel2, 1, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 1;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(1087, 700);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// pictureBox1
			// 
			this->pictureBox1->CheckForIllegalCrossThreadCalls = false;
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(0, 0);
			this->pictureBox1->Margin = System::Windows::Forms::Padding(0);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(700, 700);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// tableLayoutPanel2
			// 
			this->tableLayoutPanel2->ColumnCount = 1;
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle()));
			this->tableLayoutPanel2->Controls->Add(this->tcpstatus, 0, 0);
			this->tableLayoutPanel2->Controls->Add(this->robotbox, 0, 1);
			this->tableLayoutPanel2->Controls->Add(this->xLabel, 0, 2);
			this->tableLayoutPanel2->Controls->Add(this->yLabel, 0, 3);
			this->tableLayoutPanel2->Controls->Add(this->rpmLabel, 0, 5);
			this->tableLayoutPanel2->Controls->Add(this->mapstate, 0, 6);
			this->tableLayoutPanel2->Controls->Add(this->heading, 0, 4);
			this->tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel2->Location = System::Drawing::Point(700, 0);
			this->tableLayoutPanel2->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
			this->tableLayoutPanel2->RowCount = 8;
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 33.33443F)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 33.33445F)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 30)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 33.33112F)));
			this->tableLayoutPanel2->Size = System::Drawing::Size(387, 700);
			this->tableLayoutPanel2->TabIndex = 1;
			// 
			// tcpstatus
			// 
			this->tcpstatus->CheckForIllegalCrossThreadCalls = false;
			this->tcpstatus->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tcpstatus->Location = System::Drawing::Point(3, 3);
			this->tcpstatus->Multiline = true;
			this->tcpstatus->Name = L"tcpstatus";
			this->tcpstatus->ReadOnly = true;
			this->tcpstatus->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->tcpstatus->Size = System::Drawing::Size(381, 190);
			this->tcpstatus->TabIndex = 1;
			// 
			// robotbox
			// 
			this->robotbox->CheckForIllegalCrossThreadCalls = false;
			this->robotbox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->robotbox->Location = System::Drawing::Point(3, 199);
			this->robotbox->Multiline = true;
			this->robotbox->Name = L"robotbox";
			this->robotbox->ReadOnly = true;
			this->robotbox->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->robotbox->Size = System::Drawing::Size(381, 190);
			this->robotbox->TabIndex = 2;
			// 
			// xLabel
			// 
			this->xLabel->CheckForIllegalCrossThreadCalls = false;
			this->xLabel->AutoSize = true;
			this->xLabel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->xLabel->Location = System::Drawing::Point(3, 392);
			this->xLabel->Name = L"xLabel";
			this->xLabel->Size = System::Drawing::Size(381, 20);
			this->xLabel->TabIndex = 3;
			this->xLabel->Text = L"x";
			// 
			// yLabel
			// 
			this->yLabel->CheckForIllegalCrossThreadCalls = false;
			this->yLabel->AutoSize = true;
			this->yLabel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->yLabel->Location = System::Drawing::Point(3, 412);
			this->yLabel->Name = L"yLabel";
			this->yLabel->Size = System::Drawing::Size(381, 20);
			this->yLabel->TabIndex = 4;
			this->yLabel->Text = L"y";
			// 
			// rpmLabel
			// 
			this->rpmLabel->CheckForIllegalCrossThreadCalls = false;
			this->rpmLabel->AutoSize = true;
			this->rpmLabel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->rpmLabel->Location = System::Drawing::Point(3, 452);
			this->rpmLabel->Name = L"rpmLabel";
			this->rpmLabel->Size = System::Drawing::Size(381, 20);
			this->rpmLabel->TabIndex = 5;
			this->rpmLabel->Text = L"rpm";
			// 
			// mapstate
			// 
			this->mapstate->AutoSize = true;
			this->mapstate->Location = System::Drawing::Point(3, 475);
			this->mapstate->Name = L"mapstate";
			this->mapstate->Size = System::Drawing::Size(47, 17);
			this->mapstate->TabIndex = 6;
			this->mapstate->Text = L"Map";
			this->mapstate->UseVisualStyleBackColor = true;
			this->mapstate->CheckedChanged += gcnew System::EventHandler(this, &MyForm::mapstate_CheckedChanged);
			// 
			// heading
			// 
			this->heading->CheckForIllegalCrossThreadCalls = false;
			this->heading->AutoSize = true;
			this->heading->Dock = System::Windows::Forms::DockStyle::Fill;
			this->heading->Location = System::Drawing::Point(3, 432);
			this->heading->Name = L"heading";
			this->heading->Size = System::Drawing::Size(381, 20);
			this->heading->TabIndex = 7;
			this->heading->Text = L"heading";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1087, 700);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"MyForm";
			this->Text = L"lidar c++ controller";
			this->Shown += gcnew System::EventHandler(this, &MyForm::MyForm_Shown);
			this->tableLayoutPanel1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->tableLayoutPanel2->ResumeLayout(false);
			this->tableLayoutPanel2->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

		void maintcp();

		SOCKET Connect();

		bool tcpchangebuffer(char msgsearch[], SOCKET client);

		int tcpmsgsend(char message[], int length, SOCKET client);

		int tcpmsgreceive(SOCKET client, int offset);

		void updaterecvbuffer(SOCKET client, int offset);

		void setMotorspeeds(motorspeed *ms);

		void CheckpacketValidity(SOCKET client);

	private: System::Void MyForm_Shown(System::Object^  sender, System::EventArgs^  e)
	{
		
		//MyForm^ myform = gcnew MyForm;
		Thread^ t = gcnew Thread(gcnew ThreadStart(this, &MyForm::maintcp));
		t->Start();
		/*Thread^ picref = gcnew Thread(gcnew ThreadStart(this, &MyForm::picref));
		picref->Start();*/
	}
	private: System::Void mapstate_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		if (mapstate->Checked) {
			mapping = true;
		}
		else {
			mapping = false;
		}
	}
}
; }






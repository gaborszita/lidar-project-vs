/* 
* Copyright (C) 2017-2020 Gabor Szita
* 
* This file is part of lidar project.
*
* Lidar project is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 3 as published by
* the Free Software Foundation.
*
* Lidar project is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with lidar project.  If not, see https://www.gnu.org/licenses/ 
*/ 

#include "MyForm.h"
#include "RobotComm.h"
using namespace System;
using namespace System::Windows::Forms;

//this->rpmLabel->CheckForIllegalCrossThreadCalls = false; tcpstat, robotbox, x, y, rpm, picBox1


[STAThreadAttribute]

void main(/*array<String^>^ args*/) {
	//FreeConsole();
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Project1::MyForm form;
	Application::Run(%form);
}

namespace Project1
{
	/*MyForm::MyForm(void)
	{

	}*/
	MyForm::MyForm(void)
	{
		InitializeComponent();
		//
		//TODO: Add the constructor code here
		//
	}

	MyForm::~MyForm()
	{
		if (components)
		{
			delete components;
		}
	}

	void MyForm::InitializeComponent(void)
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

		this->pictureBox1->CheckForIllegalCrossThreadCalls = false;
		this->tcpstatus->CheckForIllegalCrossThreadCalls = false;
		this->xLabel->CheckForIllegalCrossThreadCalls = false;
		this->rpmLabel->CheckForIllegalCrossThreadCalls = false;
		this->heading->CheckForIllegalCrossThreadCalls = false;
		this->yLabel->CheckForIllegalCrossThreadCalls = false;
		this->robotbox->CheckForIllegalCrossThreadCalls = false;
	}

	System::Void MyForm::MyForm_Shown(System::Object^  sender, System::EventArgs^  e)
	{

		MyForm^ myform = gcnew MyForm;
		Thread^ t = gcnew Thread(gcnew ThreadStart(this, &MyForm::maintcp));
		t->Start();

		/*Thread^ picref = gcnew Thread(gcnew ThreadStart(this, &MyForm::picref));
		picref->Start();*/
	}

	System::Void MyForm::mapstate_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		if (mapstate->Checked) {
			mapEnabled = true;
		}
		else {
			mapEnabled = false;
		}
	}

	void MyForm::maintcp()
	{
		RobotComm robotComm(this);
		robotComm.maintcp();
	}
}
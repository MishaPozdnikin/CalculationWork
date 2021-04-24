#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "matrix.h"
#include "timer.h"
#include "mpi.h"

#define WRITING_DATA

int main(int argc, char* argv[])
{
	int ProcNum, ProcRank, RecvRank;

	size_t n = 0;
	int res_location = 0;
	MPI_Status Status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
		
	if (ProcRank == 0)
	{
		srand(time(NULL));
		matrix A(160, 154), B(154, 73), C(160, 73);
		std::cout << "Enter 1 if you want to fill matrixes manually (else 0): " << std::endl;
		int wantToFill = 0;
		std::cin >> wantToFill;

		std::cout << std::endl << "------------------------------------------------------------" << std::endl;
		if (wantToFill) {
			std::cin >> A;
			std::cout << "Fill matrix A: " << std::endl;
			std::cin >> B;
			std::cout << "Fill matrix B: " << std::endl;
		}
		else {
			for (int i = 0; i < 160; i++) {
				for (int j = 0; j < 154; j++) {
					A[i][j] =  i + 1.0;
				}
			}

			for (int i = 0; i < 154; i++) {
				for (int j = 0; j < 73; j++) {
					B[i][j] = 1.0;
				}
			}
		}

		// sequential algorithm
		timer s_timer, p_timer;
		s_timer.startTimer();
		matrix res_seq = A * B;

		std::ofstream seq("TEMP_RESULT_SEQ.txt");
		std::ifstream seqInput("TEMP_RESULT_SEQ.txt");

		seq << res_seq;

		seq << std::endl << std::endl;

		for (int i = 1; i <= 73; ++i) {
			seq << std::setw(13) << i;
		}

		seq.close();

		std::ofstream RESULT_SEQ("RESULT_SEQ.txt");
		std::string line_seq;
		int counter_seq = 0;
		while (std::getline(seqInput, line_seq)) {
			const std::string str_num = std::to_string(++counter_seq);
			if (counter_seq <= 160) {
				line_seq.append(" - ");
				line_seq.append(str_num);
				line_seq.append("\n");
				RESULT_SEQ << line_seq;
			}
			else {
				RESULT_SEQ << line_seq;
			}
		}
		seqInput.close();
		RESULT_SEQ.close();

		s_timer.endTimer();
		
		///

		p_timer.startTimer();

		for (int i = 1; i < 8; ++i) {
			MPI_Send(&A[20*i][0], 154 * 20, MPI_LONG_DOUBLE, i, i, MPI_COMM_WORLD);
		}

		matrix tmpA(20, 154);

		std::vector<matrix> tmpB;
		for (int i = 0; i < 7; i++) {
			tmpB.push_back(matrix(154, 9));
		}
		tmpB.push_back(matrix(154, 10));

		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 154; j++) {
				tmpA[i][j] = A[i][j];
			}
		}

		for (int i = 0; i < 154; i++) {
			for (int j = 0; j < 9; j++) {
				for (int k = 0; k < 7; k++) {
					tmpB[k][i][j] = B[i][j + 9 * k];
				}
				if (j == 8) {
					tmpB[8][i][9] = B[i][72];
				}
			}
		}
		
		std::vector<matrix> tmpC;
		for (int i = 0; i < 8; i++) {
			tmpC.push_back(tmpA * tmpB[i]);
		}
		
		for (int i = 1; i < 8; ++i) {
			MPI_Send(&tmpB[i][0][0], 154 * 9, MPI_LONG_DOUBLE, 0, i + 10, MPI_COMM_WORLD);
		}		

		std::vector<matrix> vC;
		for (int i = 1; i < 8; i++) {
			vC.push_back(matrix(20, 73));
			MPI_Recv(&vC[i][0][0], 20 * 73, MPI_LONG_DOUBLE, i, 30 + i, MPI_COMM_WORLD, &Status);
			C = vC[i];
		}

		std::ofstream fC("TEMP_RESULT_PAR.txt");
		std::ifstream fCInput("TEMP_RESULT_PAR.txt");

		fC << C;
		
		fC << std::endl << std::endl;
		
		for (int i = 1; i <= 73; ++i) {
			fC << std::setw(13) << i;
		}

		fC.close();
		
		std::ofstream RESULT("RESULT_PAR.txt");
		std::string line;
		int counter = 0;
		while (std::getline(fCInput, line)) {
			const std::string str_num = std::to_string(++counter);
			if (counter <= 160) {
				line.append(" - ");
				line.append(str_num);
				line.append("\n");
				RESULT << line;
			}
			else {
				RESULT << line;
			}
		}
		fCInput.close();
		RESULT.close();

		p_timer.endTimer();

		std::cout << "Sequential time: " << s_timer << std::endl << std::endl;
		std::cout << "Parallel time: " << p_timer << std::endl << std::endl << std::endl;
	}
	else if (ProcRank == 1)
	{
		matrix tmpA(20, 154);
		std::vector<matrix> tmpB;

		for (int i = 0; i < 7; i++) {
			tmpB.push_back(matrix(154, 9));
		}
		tmpB.push_back(matrix(154, 10));

		MPI_Recv(&tmpA[0][0], 154 * 20, MPI_LONG_DOUBLE, 0, ProcRank, MPI_COMM_WORLD, &Status);
		MPI_Recv(&B[0][0], 154 * 73, MPI_LONG_DOUBLE, 0, ProcRank+10, MPI_COMM_WORLD, &Status);

		matrix tmpC12 = tmpA * tmpB2;

		matrix tmpC11 = tmpA * tmpB1; //(20, 9)

		matrix tmpC16 = tmpA * tmpB6;

		matrix tmpC14 = tmpA * tmpB4;

		matrix tmpC15 = tmpA * tmpB5;

		matrix tmpC17 = tmpA * tmpB7;

		matrix tmpC18 = tmpA * tmpB8;

		matrix tmpC13 = tmpA * tmpB3;

		matrix tmpC1(20, 73);

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				tmpC1[i][j] = tmpC11[i][j];
				tmpC1[i][9 + j] = tmpC12[i][j];
				tmpC1[i][18 + j] = tmpC13[i][j];
				tmpC1[i][27 + j] = tmpC14[i][j];
				tmpC1[i][36 + j] = tmpC15[i][j];
				tmpC1[i][45 + j] = tmpC16[i][j];
				tmpC1[i][54 + j] = tmpC17[i][j];
				tmpC1[i][63 + j] = tmpC18[i][j];

				if (j == 8)
				{
					tmpC1[i][72] = tmpC18[i][9];
				}
			}
		}

		MPI_Send(&tmpC1[0][0], 20 * 73, MPI_LONG_DOUBLE, 0, 31, MPI_COMM_WORLD);
	}
	else if (ProcRank == 2)
	{
		matrix tmpA(20, 154);
		matrix B(154, 73);
		matrix tmpB1(154, 9);
		matrix tmpB2(154, 9);
		matrix tmpB3(154, 9);
		matrix tmpB4(154, 9);
		matrix tmpB5(154, 9);
		matrix tmpB6(154, 9);
		matrix tmpB7(154, 9);
		matrix tmpB8(154, 10);

		MPI_Recv(&tmpA[0][0], 154 * 20, MPI_LONG_DOUBLE, 0, ProcRank, MPI_COMM_WORLD, &Status);
		MPI_Recv(&B[0][0], 154 * 73, MPI_LONG_DOUBLE, 0, ProcRank + 10, MPI_COMM_WORLD, &Status);

		for (int i = 0; i < 154; i++) {
			for (int j = 0; j < 9; j++) {
				tmpB1[i][j] = B[i][j];
				tmpB2[i][j] = B[i][j + 9];
				tmpB3[i][j] = B[i][j + 18];
				tmpB4[i][j] = B[i][j + 27];
				tmpB5[i][j] = B[i][j + 36];
				tmpB6[i][j] = B[i][j + 45];
				tmpB7[i][j] = B[i][j + 54];
				tmpB8[i][j] = B[i][j + 63];

				if (j == 8) {
					tmpB8[i][9] = B[i][72];
				}
			}
		}

		matrix tmpC12 = tmpA * tmpB2;

		matrix tmpC11 = tmpA * tmpB1;

		matrix tmpC16 = tmpA * tmpB6;

		matrix tmpC14 = tmpA * tmpB4;

		matrix tmpC15 = tmpA * tmpB5;

		matrix tmpC17 = tmpA * tmpB7;

		matrix tmpC18 = tmpA * tmpB8;

		matrix tmpC13 = tmpA * tmpB3;

		matrix tmpC1(20, 73);

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				tmpC1[i][j] = tmpC11[i][j];
				tmpC1[i][9 + j] = tmpC12[i][j];
				tmpC1[i][18 + j] = tmpC13[i][j];
				tmpC1[i][27 + j] = tmpC14[i][j];
				tmpC1[i][36 + j] = tmpC15[i][j];
				tmpC1[i][45 + j] = tmpC16[i][j];
				tmpC1[i][54 + j] = tmpC17[i][j];
				tmpC1[i][63 + j] = tmpC18[i][j];

				if (j == 8)
				{
					tmpC1[i][72] = tmpC18[i][9];
				}
			}
		}

		MPI_Send(&tmpC1[0][0], 20 * 73, MPI_LONG_DOUBLE, 0, 32, MPI_COMM_WORLD);
	}
	else if (ProcRank == 3)
	{
		matrix tmpA(20, 154);
		matrix B(154, 73);
		matrix tmpB1(154, 9);
		matrix tmpB2(154, 9);
		matrix tmpB3(154, 9);
		matrix tmpB4(154, 9);
		matrix tmpB5(154, 9);
		matrix tmpB6(154, 9);
		matrix tmpB7(154, 9);
		matrix tmpB8(154, 10);

		MPI_Recv(&tmpA[0][0], 154 * 20, MPI_LONG_DOUBLE, 0, ProcRank, MPI_COMM_WORLD, &Status);
		MPI_Recv(&B[0][0], 154 * 73, MPI_LONG_DOUBLE, 0, ProcRank + 10, MPI_COMM_WORLD, &Status);

		for (int i = 0; i < 154; i++) {
			for (int j = 0; j < 9; j++) {
				tmpB1[i][j] = B[i][j];
				tmpB2[i][j] = B[i][j + 9];
				tmpB3[i][j] = B[i][j + 18];
				tmpB4[i][j] = B[i][j + 27];
				tmpB5[i][j] = B[i][j + 36];
				tmpB6[i][j] = B[i][j + 45];
				tmpB7[i][j] = B[i][j + 54];
				tmpB8[i][j] = B[i][j + 63];

				if (j == 8) {
					tmpB8[i][9] = B[i][72];
				}
			}
		}

		matrix tmpC12 = tmpA * tmpB2;

		matrix tmpC11 = tmpA * tmpB1;

		matrix tmpC16 = tmpA * tmpB6;

		matrix tmpC14 = tmpA * tmpB4;

		matrix tmpC15 = tmpA * tmpB5;

		matrix tmpC17 = tmpA * tmpB7;

		matrix tmpC18 = tmpA * tmpB8;

		matrix tmpC13 = tmpA * tmpB3;

		matrix tmpC1(20, 73);

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				tmpC1[i][j] = tmpC11[i][j];
				tmpC1[i][9 + j] = tmpC12[i][j];
				tmpC1[i][18 + j] = tmpC13[i][j];
				tmpC1[i][27 + j] = tmpC14[i][j];
				tmpC1[i][36 + j] = tmpC15[i][j];
				tmpC1[i][45 + j] = tmpC16[i][j];
				tmpC1[i][54 + j] = tmpC17[i][j];
				tmpC1[i][63 + j] = tmpC18[i][j];

				if (j == 8)
				{
					tmpC1[i][72] = tmpC18[i][9];
				}
			}
		}

		MPI_Send(&tmpC1[0][0], 20 * 73, MPI_LONG_DOUBLE, 0, 33, MPI_COMM_WORLD);
	}
	else if (ProcRank == 4)
	{
		matrix tmpA(20, 154);
		matrix B(154, 73);
		matrix tmpB1(154, 9);
		matrix tmpB2(154, 9);
		matrix tmpB3(154, 9);
		matrix tmpB4(154, 9);
		matrix tmpB5(154, 9);
		matrix tmpB6(154, 9);
		matrix tmpB7(154, 9);
		matrix tmpB8(154, 10);

		MPI_Recv(&tmpA[0][0], 154 * 20, MPI_LONG_DOUBLE, 0, ProcRank, MPI_COMM_WORLD, &Status);
		MPI_Recv(&B[0][0], 154 * 73, MPI_LONG_DOUBLE, 0, ProcRank + 10, MPI_COMM_WORLD, &Status);

		for (int i = 0; i < 154; i++) {
			for (int j = 0; j < 9; j++) {
				tmpB1[i][j] = B[i][j];
				tmpB2[i][j] = B[i][j + 9];
				tmpB3[i][j] = B[i][j + 18];
				tmpB4[i][j] = B[i][j + 27];
				tmpB5[i][j] = B[i][j + 36];
				tmpB6[i][j] = B[i][j + 45];
				tmpB7[i][j] = B[i][j + 54];
				tmpB8[i][j] = B[i][j + 63];

				if (j == 8) {
					tmpB8[i][9] = B[i][72];
				}
			}
		}

		matrix tmpC12 = tmpA * tmpB2;

		matrix tmpC11 = tmpA * tmpB1;

		matrix tmpC16 = tmpA * tmpB6;

		matrix tmpC14 = tmpA * tmpB4;

		matrix tmpC15 = tmpA * tmpB5;

		matrix tmpC17 = tmpA * tmpB7;

		matrix tmpC18 = tmpA * tmpB8;

		matrix tmpC13 = tmpA * tmpB3;

		matrix tmpC1(20, 73);

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				tmpC1[i][j] = tmpC11[i][j];
				tmpC1[i][9 + j] = tmpC12[i][j];
				tmpC1[i][18 + j] = tmpC13[i][j];
				tmpC1[i][27 + j] = tmpC14[i][j];
				tmpC1[i][36 + j] = tmpC15[i][j];
				tmpC1[i][45 + j] = tmpC16[i][j];
				tmpC1[i][54 + j] = tmpC17[i][j];
				tmpC1[i][63 + j] = tmpC18[i][j];

				if (j == 8)
				{
					tmpC1[i][72] = tmpC18[i][9];
				}
			}
		}

		MPI_Send(&tmpC1[0][0], 20 * 73, MPI_LONG_DOUBLE, 0, 34, MPI_COMM_WORLD);
	}
	else if (ProcRank == 5)
	{
		matrix tmpA(20, 154);
		matrix B(154, 73);
		matrix tmpB1(154, 9);
		matrix tmpB2(154, 9);
		matrix tmpB3(154, 9);
		matrix tmpB4(154, 9);
		matrix tmpB5(154, 9);
		matrix tmpB6(154, 9);
		matrix tmpB7(154, 9);
		matrix tmpB8(154, 10);

		MPI_Recv(&tmpA[0][0], 154 * 20, MPI_LONG_DOUBLE, 0, ProcRank, MPI_COMM_WORLD, &Status);
		MPI_Recv(&B[0][0], 154 * 73, MPI_LONG_DOUBLE, 0, ProcRank + 10, MPI_COMM_WORLD, &Status);

		for (int i = 0; i < 154; i++) {
			for (int j = 0; j < 9; j++) {
				tmpB1[i][j] = B[i][j];
				tmpB2[i][j] = B[i][j + 9];
				tmpB3[i][j] = B[i][j + 18];
				tmpB4[i][j] = B[i][j + 27];
				tmpB5[i][j] = B[i][j + 36];
				tmpB6[i][j] = B[i][j + 45];
				tmpB7[i][j] = B[i][j + 54];
				tmpB8[i][j] = B[i][j + 63];

				if (j == 8) {
					tmpB8[i][9] = B[i][72];
				}
			}
		}

		matrix tmpC12 = tmpA * tmpB2;

		matrix tmpC11 = tmpA * tmpB1;

		matrix tmpC16 = tmpA * tmpB6;

		matrix tmpC14 = tmpA * tmpB4;

		matrix tmpC15 = tmpA * tmpB5;

		matrix tmpC17 = tmpA * tmpB7;

		matrix tmpC18 = tmpA * tmpB8;

		matrix tmpC13 = tmpA * tmpB3;

		matrix tmpC1(20, 73);

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				tmpC1[i][j] = tmpC11[i][j];
				tmpC1[i][9 + j] = tmpC12[i][j];
				tmpC1[i][18 + j] = tmpC13[i][j];
				tmpC1[i][27 + j] = tmpC14[i][j];
				tmpC1[i][36 + j] = tmpC15[i][j];
				tmpC1[i][45 + j] = tmpC16[i][j];
				tmpC1[i][54 + j] = tmpC17[i][j];
				tmpC1[i][63 + j] = tmpC18[i][j];

				if (j == 8)
				{
					tmpC1[i][72] = tmpC18[i][9];
				}
			}
		}

		MPI_Send(&tmpC1[0][0], 20 * 73, MPI_LONG_DOUBLE, 0, 35, MPI_COMM_WORLD);
	}
	else if (ProcRank == 6)
	{
		matrix tmpA(20, 154);
		matrix B(154, 73);
		matrix tmpB1(154, 9);
		matrix tmpB2(154, 9);
		matrix tmpB3(154, 9);
		matrix tmpB4(154, 9);
		matrix tmpB5(154, 9);
		matrix tmpB6(154, 9);
		matrix tmpB7(154, 9);
		matrix tmpB8(154, 10);

		MPI_Recv(&tmpA[0][0], 154 * 20, MPI_LONG_DOUBLE, 0, ProcRank, MPI_COMM_WORLD, &Status);
		MPI_Recv(&B[0][0], 154 * 73, MPI_LONG_DOUBLE, 0, ProcRank + 10, MPI_COMM_WORLD, &Status);

		for (int i = 0; i < 154; i++) {
			for (int j = 0; j < 9; j++) {
				tmpB1[i][j] = B[i][j];
				tmpB2[i][j] = B[i][j + 9];
				tmpB3[i][j] = B[i][j + 18];
				tmpB4[i][j] = B[i][j + 27];
				tmpB5[i][j] = B[i][j + 36];
				tmpB6[i][j] = B[i][j + 45];
				tmpB7[i][j] = B[i][j + 54];
				tmpB8[i][j] = B[i][j + 63];

				if (j == 8) {
					tmpB8[i][9] = B[i][72];
				}
			}
		}

		matrix tmpC12 = tmpA * tmpB2;

		matrix tmpC11 = tmpA * tmpB1;

		matrix tmpC16 = tmpA * tmpB6;

		matrix tmpC14 = tmpA * tmpB4;

		matrix tmpC15 = tmpA * tmpB5;

		matrix tmpC17 = tmpA * tmpB7;

		matrix tmpC18 = tmpA * tmpB8;

		matrix tmpC13 = tmpA * tmpB3;

		matrix tmpC1(20, 73);

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				tmpC1[i][j] = tmpC11[i][j];
				tmpC1[i][9 + j] = tmpC12[i][j];
				tmpC1[i][18 + j] = tmpC13[i][j];
				tmpC1[i][27 + j] = tmpC14[i][j];
				tmpC1[i][36 + j] = tmpC15[i][j];
				tmpC1[i][45 + j] = tmpC16[i][j];
				tmpC1[i][54 + j] = tmpC17[i][j];
				tmpC1[i][63 + j] = tmpC18[i][j];

				if (j == 8)
				{
					tmpC1[i][72] = tmpC18[i][9];
				}
			}
		}

		MPI_Send(&tmpC1[0][0], 20 * 73, MPI_LONG_DOUBLE, 0, 36, MPI_COMM_WORLD);
	}
	else if (ProcRank == 7)
	{
		matrix tmpA(20, 154);
		matrix B(154, 73);
		matrix tmpB1(154, 9);
		matrix tmpB2(154, 9);
		matrix tmpB3(154, 9);
		matrix tmpB4(154, 9);
		matrix tmpB5(154, 9);
		matrix tmpB6(154, 9);
		matrix tmpB7(154, 9);
		matrix tmpB8(154, 10);

		MPI_Recv(&tmpA[0][0], 154 * 20, MPI_LONG_DOUBLE, 0, ProcRank, MPI_COMM_WORLD, &Status);
		MPI_Recv(&B[0][0], 154 * 73, MPI_LONG_DOUBLE, 0, ProcRank + 10, MPI_COMM_WORLD, &Status);

		for (int i = 0; i < 154; i++) {
			for (int j = 0; j < 9; j++) {
				tmpB1[i][j] = B[i][j];
				tmpB2[i][j] = B[i][j + 9];
				tmpB3[i][j] = B[i][j + 18];
				tmpB4[i][j] = B[i][j + 27];
				tmpB5[i][j] = B[i][j + 36];
				tmpB6[i][j] = B[i][j + 45];
				tmpB7[i][j] = B[i][j + 54];
				tmpB8[i][j] = B[i][j + 63];

				if (j == 8) {
					tmpB8[i][9] = B[i][72];
				}
			}
		}

		matrix tmpC12 = tmpA * tmpB2;

		matrix tmpC11 = tmpA * tmpB1;

		matrix tmpC16 = tmpA * tmpB6;

		matrix tmpC14 = tmpA * tmpB4;

		matrix tmpC15 = tmpA * tmpB5;

		matrix tmpC17 = tmpA * tmpB7;

		matrix tmpC18 = tmpA * tmpB8;

		matrix tmpC13 = tmpA * tmpB3;

		matrix tmpC1(20, 73);

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				tmpC1[i][j] = tmpC11[i][j];
				tmpC1[i][9 + j] = tmpC12[i][j];
				tmpC1[i][18 + j] = tmpC13[i][j];
				tmpC1[i][27 + j] = tmpC14[i][j];
				tmpC1[i][36 + j] = tmpC15[i][j];
				tmpC1[i][45 + j] = tmpC16[i][j];
				tmpC1[i][54 + j] = tmpC17[i][j];
				tmpC1[i][63 + j] = tmpC18[i][j];

				if (j == 8)
				{
					tmpC1[i][72] = tmpC18[i][9];
				}
			}
		}

		MPI_Send(&tmpC1[0][0], 20 * 73, MPI_LONG_DOUBLE, 0, 37, MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	return 0;
}

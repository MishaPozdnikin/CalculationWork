#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
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
					B[i][j] = j + 1.0;
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
		std::cout << "1" << std::endl;
		std::vector<matrix> tmpB;
		for (int i = 0; i < 7; i++) {
			tmpB.push_back(matrix(154, 9));
		}
		tmpB.push_back(matrix(154, 10));
		std::cout << "2" << std::endl;

		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 154; j++) {
				tmpA[i][j] = A[i][j];
			}
		}
		std::cout << "3" << std::endl;

		for (int i = 0; i < 154; i++) {
			for (int j = 0; j < 9; j++) {
				for (int k = 0; k < 8; k++) {
					tmpB[k][i][j] = B[i][j + 9 * k];
				}
				if (j == 8) {
					tmpB[7][i][9] = B[i][72];
				}
			}
		}
		std::cout << "4" << std::endl;
		
		matrix tmpC11 = tmpA * tmpB[0]; //(20, 9)
		matrix tmpC16 = tmpA * tmpB[5];
		matrix tmpC14 = tmpA * tmpB[3];
		matrix tmpC15 = tmpA * tmpB[4];
		matrix tmpC17 = tmpA * tmpB[6];
		matrix tmpC18 = tmpA * tmpB[7];
		matrix tmpC13 = tmpA * tmpB[2];
		matrix tmpC12 = tmpA * tmpB[1];
		std::cout << "5" << std::endl;

		matrix tmpC1(20, 73);
		matrix tmpC2(20, 73);
		matrix tmpC3(20, 73);
		matrix tmpC4(20, 73);
		matrix tmpC5(20, 73);
		matrix tmpC6(20, 73);
		matrix tmpC7(20, 73);
		matrix tmpC8(20, 73);

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
		
		for (int i = 1; i < 7; ++i) {
			MPI_Send(&tmpB[i][0][0], 154 * 9, MPI_LONG_DOUBLE, 1, 10, MPI_COMM_WORLD);
		}
		MPI_Send(&tmpB[7][0][0], 154 * 10, MPI_LONG_DOUBLE, 1, 17, MPI_COMM_WORLD);
		MPI_Recv(&tmpC2[0][0], 20 * 73, MPI_LONG_DOUBLE, 1, 31, MPI_COMM_WORLD, &Status);
		MPI_Recv(&tmpC3[0][0], 20 * 73, MPI_LONG_DOUBLE, 2, 31, MPI_COMM_WORLD, &Status);
		MPI_Recv(&tmpC4[0][0], 20 * 73, MPI_LONG_DOUBLE, 3, 31, MPI_COMM_WORLD, &Status);
		MPI_Recv(&tmpC5[0][0], 20 * 73, MPI_LONG_DOUBLE, 4, 31, MPI_COMM_WORLD, &Status);
		MPI_Recv(&tmpC6[0][0], 20 * 73, MPI_LONG_DOUBLE, 5, 31, MPI_COMM_WORLD, &Status);
		MPI_Recv(&tmpC7[0][0], 20 * 73, MPI_LONG_DOUBLE, 6, 31, MPI_COMM_WORLD, &Status);
		MPI_Recv(&tmpC8[0][0], 20 * 73, MPI_LONG_DOUBLE, 7, 31, MPI_COMM_WORLD, &Status);

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 73; ++j)
			{
				C[i][j] = tmpC1[i][j];
				C[i + 20][j] = tmpC2[i][j];
				C[i + 40][j] = tmpC3[i][j];
				C[i + 60][j] = tmpC4[i][j];
				C[i + 80][j] = tmpC5[i][j];
				C[i + 100][j] = tmpC6[i][j];
				C[i + 120][j] = tmpC7[i][j];
				C[i + 140][j] = tmpC8[i][j];
			}
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

		std::cout << (C == res_seq);
		std::cout << "Sequential time: " << s_timer << std::endl << std::endl;
		std::cout << "Parallel time: " << p_timer << std::endl << std::endl << std::endl;
	}
	else
	{
		matrix tmpA(20, 154);
		matrix tmpB(154, 9);
		std::vector<matrix> tmpC;
		matrix C(20, 73);
		matrix tmpB10(154, 10);
		matrix res;
		std::vector<int> Hamilton{ 0,1,2,7,6,4,3,5 };
		auto pos = std::find(Hamilton.begin(), Hamilton.end(), ProcRank) - 1;
		MPI_Recv(&tmpA[0][0], 154 * 20, MPI_LONG_DOUBLE, 0, ProcRank, MPI_COMM_WORLD, &Status);
		MPI_Recv(&tmpB[0][0], 154 * 9, MPI_LONG_DOUBLE, *pos, 10, MPI_COMM_WORLD, &Status);
		
		for (int i = 0; i < 7; i++) {
			res = tmpA * tmpB;
			tmpC.push_back(res);
			if (ProcRank != 5) {
				MPI_Send(&tmpB[0][0], 154 * 9, MPI_LONG_DOUBLE, *(pos + 2), 10, MPI_COMM_WORLD);
			}
		}

		MPI_Recv(&tmpB10[0][0], 154 * 10, MPI_LONG_DOUBLE, *pos, 17, MPI_COMM_WORLD, &Status);
		//std::cout << tmpB10;
		res = tmpA * tmpB10;
		tmpC.push_back(res);
		if (ProcRank != 5) {
			MPI_Send(&tmpB10[0][0], 154 * 10, MPI_LONG_DOUBLE, *(pos + 2), 17, MPI_COMM_WORLD);
		}
		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				C[i][j] = tmpC[0][i][j];
				C[i][9 + j] = tmpC[1][i][j];
				C[i][18 + j] = tmpC[2][i][j];
				C[i][27 + j] = tmpC[3][i][j];
				C[i][36 + j] = tmpC[4][i][j];
				C[i][45 + j] = tmpC[5][i][j];
				C[i][54 + j] = tmpC[6][i][j];
				C[i][63 + j] = tmpC[7][i][j];

				if (j == 8)
				{
					C[i][72] = tmpC[7][i][9];
				}
			}
		}

		MPI_Send(&C[0][0], 20 * 73, MPI_LONG_DOUBLE, 0, 31, MPI_COMM_WORLD);
	}
	MPI_Finalize();

	return 0;
}

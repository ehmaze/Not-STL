#ifndef CODER_HPP
#define CODER_HPP

#include <string>
#include <vector>
#include <cmath>
#include <exception>
#include <algorithm>
#include <iostream>

class Coder {
public:

    Coder(int p_in, int n_in, int M_in, int d_in):
		p{p_in}, n{n_in}, M{M_in}, d{d_in}{}

	virtual std::string encode(std::string codeword) const = 0;

	virtual std::string decode(std::string received) const = 0;

	virtual std::string generate() const = 0;

protected:
	std::vector<std::vector<int>> generator;
	int p;
	int n;
	int M;
	int d;
};

class Hamming : public Coder {
	int h;
public:
	// disable default ctor, doesnt make sense why youd call this
	Hamming() = delete;

	Hamming(int p_in, int h_in) :
		Coder(p_in, 0, 0, 3), h{h_in} {
		// must be able to create the field Z_p
		if (!is_prime(p)) {
			throw std::runtime_error("p in not prime. Therefore Z_p is not a field, which renders many of the methods used in Hamming codes obsolete");
		}
		n = static_cast<int>((pow(static_cast<double>(p), static_cast<double>(h)) - 1) / (p - 1));
		M = n - h;

		// construct matrix
		generator.resize(h, std::vector<int>(n));

		int index = 0;
		int cur_num = 1;
		while (index < n) {
			// check if most significant bit is 1
			if (most_signigicant_bit_is_one(cur_num)) {
				auto rep = get_p_ary_representation(cur_num);
				int rep_index = 0;
				for (auto& row : generator) {
					row[index] = rep[rep_index];
					rep_index++;
				}
				index++;
			}
			cur_num++;
		}
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < n; j++) {
				std::cout << generator[i][j] << " ";
			}
			std::cout << std::endl;
		}
 	}

	std::string encode(std::string codeword) const {
		return "";
	}

	std::string decode(std::string received) const {
		return "";
	}

	std::string generate() const {
		return "";
	}

private:
	bool is_prime(int p) {
		if (p <= 1) {
			return false;
		}
		for (int i = 2; i < p; i++) {
			if (p % i == 0) {
				return false;
			}
		}
		return true;
	}

	bool most_signigicant_bit_is_one(int num) {
		for (int i = 0; i < h; i++) {
			if (num % p == 0) {
				num /= p;
				continue;
			} else if (num % p == 1) {
				return true;
			} else {
				return false;
			}
		}
		return false;
	}

	std::vector<int> get_p_ary_representation(int num) {
		std::vector<int> rep(h, 0);
		for (int i = 0; i < h; i++) {
			rep[i] = num % p;
			num /= p;
		}
		return rep;
	}
};


#endif
#ifndef CODER_HPP
#define CODER_HPP

#include <string>
#include <vector>
#include <cmath>
#include <exception>
#include <algorithm>
#include <iostream>
#include <cstdlib>

class Coder {
public:

    Coder(int p_in, int n_in, int M_in, int d_in):
		p{p_in}, n{n_in}, M{M_in}, d{d_in}{}

	virtual std::string encode(const std::string& codeword) const = 0;

	virtual std::string decode(const std::string& received) const = 0;

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
			const std::string rep = get_p_ary_representation(cur_num);
			if (most_signigicant_bit(rep) == 1) {
				int rep_index = 0;
				for (auto& row : generator) {
					row[index] = char_to_int(rep[rep_index]);
					rep_index++;
				}
				index++;
			}
			cur_num++;
		}
 	}

	std::string encode(const std::string& codeword) const {
		return "";
	}

	std::string decode(const std::string& received) const {
		std::string syndrome = compute_syndrom(received);

		// get msb from syndrom
		int msb = most_signigicant_bit(syndrome);
		for (auto& el: syndrome) {
			el = int_to_char(msb * char_to_int(el));
			el = int_to_char(char_to_int(el) % p);	
		}

		// if no syndrom its the correct word
		if (msb == 0) {
			return received;
		}

		// find where error occured
		int basis_num = 0;
		bool flag = true;
		for (int col = 0; col < n; col++) {
			for (int row = 0; row < h; row++) {
				if (generator[row][col] != char_to_int(syndrome[row])) {
					flag = false;
					break;
				}
			}
			if (flag) {
				basis_num = col;
				break;
			}
			flag = true;
		}

		//compute decoded word
		int new_val = algebraic_modulo((char_to_int(received.at(basis_num)) - msb), p);
		std::string decoded = received;
		decoded[basis_num] = int_to_char(new_val);
		return decoded; 
	}

	std::string generate() const {
		return "";
	}

private:
	bool is_prime(int p) const {
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

	int most_signigicant_bit(const std::string& v) const {
		for (const auto& el: v) {
			if (char_to_int(el) == 0) {
				continue;
			} else {
				return char_to_int(el);
			}
		}
		return 0;
	}

	const std::string get_p_ary_representation(int num) const {
		std::string rep(h, '0');
		for (int i = 0; i < h; i++) {
			rep[i] = int_to_char(num % p);
			num /= p;
		}
		std::reverse(rep.begin(), rep.end());
		return rep;
	}

	std::string compute_syndrom(const std::string& string_in) const {
		std::string syndrome(h, '0');
		for (int row = 0; row < h; row++) {
			for (int col = 0; col < n; col++) {
				int next_val = generator[row][col] * char_to_int(string_in.at(col));
				syndrome[row] = int_to_char((next_val + char_to_int(syndrome[row])) % p);
			}
		}
		return syndrome;
	}

	inline char int_to_char(int num) const {
		return '0' + num;
	}

	inline int char_to_int(char num) const {
		return num - '0';
	}

	inline int algebraic_modulo(int a, int b) const {
		return (a %= b < 0) ? a + b: a;
	}
};


#endif
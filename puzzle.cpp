#include <iostream>
#include <array>
#include <bitset>
#include <queue>

const size_t BOARD_SIZE = 14u;
const size_t ENCODED_SIZE = 13u + 9u + 5u + 1u;
const size_t BOARD_WIDTH = 6u;

uint32_t *arr;
std::queue<uint32_t> q;
uint32_t initial;

struct Position {
	int32_t x;
	int32_t y;
	
	constexpr Position(int32_t x, int32_t y) noexcept: x{x}, y{y} {}
	constexpr Position(Position const &other) noexcept: x{other.x}, y{other.y} {}
	constexpr Position(size_t index):
		x{static_cast<int32_t>(index % BOARD_WIDTH)}, y{static_cast<int32_t>(index / BOARD_WIDTH)} {}
	
	constexpr size_t to_index(void) const {
		return this->x + this->y * BOARD_WIDTH;
	}
};

constexpr Position operator+(Position const &lhs, Position const &rhs) {
	return Position{lhs.x + rhs.x, lhs.y + rhs.y};
}

class BoardState {
	std::bitset<BOARD_SIZE> rooks;
	std::bitset<BOARD_SIZE> bishops;
	std::bitset<BOARD_SIZE> knights;
	std::bitset<BOARD_SIZE> blackKnight;
	
	enum class Piece {
		ROOK, BISHOP, KNIGHT
	};
	
	bool explore_move(Position from, Position to, Piece piece) const {
		if (from.x < 0 || from.x >= BOARD_WIDTH || from.y < 0 || from.y >= 3 || (from.x >= 2 && from.y >= 2)) return false;
		size_t fromIndex = from.to_index();
		size_t toIndex = to.to_index();
		BoardState next{*this};
		
		switch (piece) {
		case Piece::ROOK:
			if (!next.rooks[fromIndex]) return false;
			next.rooks.reset(fromIndex);
			next.rooks.set(toIndex);
			break;
		case Piece::BISHOP:
			if (!next.bishops[fromIndex]) return false;
			next.bishops.reset(fromIndex);
			next.bishops.set(toIndex);
			break;
		case Piece::KNIGHT:
			if (next.knights[fromIndex]) {
				next.knights.reset(fromIndex);
				next.knights.set(toIndex);
			} else {
				if (!next.blackKnight[fromIndex]) return false;
				next.blackKnight.reset(fromIndex);
				next.blackKnight.set(toIndex);
			}
		}
		
		uint32_t encodedNext = next.encode();
		
		if (arr[encodedNext] == 0u) {
			arr[encodedNext] = this->encode();
			q.push(encodedNext);
		}
		
		if (next.blackKnight[12u]) {
			std::cout << "\nSOLUTION FOUND:\n\n";
			
			while (encodedNext != initial) {
				BoardState{encodedNext}.print();
				encodedNext = arr[encodedNext];
			}
			BoardState{encodedNext}.print();
			
			return true;
		}
		return false;
	}
	
public:
	constexpr BoardState(void) noexcept {}
	constexpr BoardState(BoardState const &other) noexcept:
		rooks{other.rooks}, bishops{other.bishops}, knights{other.knights}, blackKnight{other.blackKnight} {}
	
	constexpr void init(void) noexcept {
		this->rooks =		std::bitset<BOARD_SIZE>{0b10000011000001u};
		this->bishops =		std::bitset<BOARD_SIZE>{0b00000000011110u};
		this->knights =		std::bitset<BOARD_SIZE>{0b00111100000000u};
		this->blackKnight =	std::bitset<BOARD_SIZE>{0b00000000100000u};
	}
	
	bool is_valid(void) const noexcept {
		return this->rooks.any() || this->bishops.any() || this->knights.any() || this->blackKnight.any();
	}

	void print(void) const {
		std::cout << "+------+\n|";
		for (size_t i = 0u; i < BOARD_SIZE; ++i) {
			if (i == BOARD_WIDTH || i == 2 * BOARD_WIDTH) {
				std::cout << "|\n|";
			}
			if (this->rooks[i]) {
				std::cout << 'R';
				continue;
			}
			if (this->bishops[i]) {
				std::cout << 'B';
				continue;
			}
			if (this->knights[i]) {
				std::cout << 'N';
				continue;
			}
			if (this->blackKnight[i]) {
				std::cout << 'n';
				continue;
			}
			std::cout << ' ';
		}
		std::cout << "+---+\n+--+\n";
	}
	
	uint32_t encode(void) const {
		std::bitset<10u> bishops;
		std::bitset<6u> knights;
		std::bitset<2u> blackKnight;
		size_t bishopsIndex = 0u, knightsIndex = 0u, blackKnightIndex = 0u;
		for (size_t i = 0u; i < BOARD_SIZE; ++i) {
			if (this->rooks[i]) {
				continue;
			}
			if (bishops[bishopsIndex++] = this->bishops[i]) {
				continue;
			}
			if (knights[knightsIndex++] = this->knights[i]) {
				continue;
			}
			blackKnight[blackKnightIndex++] = this->blackKnight[i];
		}
		uint32_t ret = static_cast<uint32_t>(this->rooks.to_ulong()) >> 1u;
		ret <<= 9u;
		ret |= static_cast<uint32_t>(bishops.to_ulong()) >> 1u;
		ret <<= 5u;
		ret |= static_cast<uint32_t>(knights.to_ulong()) >> 1u;
		ret <<= 1u;
		ret |= static_cast<uint32_t>(blackKnight.to_ulong()) >> 1u;
		return ret;
	}
	
	BoardState(uint32_t encoded): BoardState() {
		std::bitset<2u> blackKnight{(encoded & 0b1u) << 1u};
		if (blackKnight.count() != 1u) {
			blackKnight.set(0u);
		}
		encoded >>= 1u;
		std::bitset<6u> knights{(encoded & 0b11111u) << 1u};
		if (knights.count() != 4u) {
			knights.set(0u);
		}
		encoded >>= 5u;
		std::bitset<10u> bishops{(encoded & 0b111111111u) << 1u};
		if (bishops.count() != 4u) {
			bishops.set(0u);
		}
		encoded >>= 9u;
		std::bitset<14u> rooks{(encoded & 0b1111111111111u) << 1u};
		if (rooks.count() != 4u) {
			rooks.set(0u);
		}
		this->rooks = rooks;
		size_t bishopsIndex = 0u, knightsIndex = 0u, blackKnightIndex = 0u;
		for (size_t i = 0u; i < BOARD_SIZE; ++i) {
			if (rooks[i]) {
				continue;
			}
			if (this->bishops[i] = bishops[bishopsIndex++]) {
				continue;
			}
			if (this->knights[i] = knights[knightsIndex++]) {
				continue;
			}
			this->blackKnight[i] = blackKnight[blackKnightIndex++];
		}
	}
	
	bool operator==(const BoardState &rhs) const {
		return this->rooks == rhs.rooks && this->bishops == rhs.bishops && this->knights == rhs.knights && this->blackKnight == rhs.blackKnight;
	}
	
	size_t getFreeIndex(void) const {
		std::bitset<BOARD_SIZE> board = this->rooks | this->bishops | this->knights | this->blackKnight;
		size_t ret = 0u;
		while (board[ret]) {
			++ret;
		}
		return ret;
	}

	bool explore(void) const {
		size_t freeIndex = this->getFreeIndex();
		Position freePosition{freeIndex};
		
		if (explore_move(freePosition + Position{-1, +0}, freePosition, Piece::ROOK)) return true;
		if (explore_move(freePosition + Position{+1, +0}, freePosition, Piece::ROOK)) return true;
		if (explore_move(freePosition + Position{+0, -1}, freePosition, Piece::ROOK)) return true;
		if (explore_move(freePosition + Position{+0, +1}, freePosition, Piece::ROOK)) return true;
		if (explore_move(freePosition + Position{-1, -1}, freePosition, Piece::BISHOP)) return true;
		if (explore_move(freePosition + Position{+1, -1}, freePosition, Piece::BISHOP)) return true;
		if (explore_move(freePosition + Position{-1, +1}, freePosition, Piece::BISHOP)) return true;
		if (explore_move(freePosition + Position{+1, +1}, freePosition, Piece::BISHOP)) return true;
		if (explore_move(freePosition + Position{-1, -2}, freePosition, Piece::KNIGHT)) return true;
		if (explore_move(freePosition + Position{+1, -2}, freePosition, Piece::KNIGHT)) return true;
		if (explore_move(freePosition + Position{-2, -1}, freePosition, Piece::KNIGHT)) return true;
		if (explore_move(freePosition + Position{+2, -1}, freePosition, Piece::KNIGHT)) return true;
		if (explore_move(freePosition + Position{-2, +1}, freePosition, Piece::KNIGHT)) return true;
		if (explore_move(freePosition + Position{+2, +1}, freePosition, Piece::KNIGHT)) return true;
		if (explore_move(freePosition + Position{-1, +2}, freePosition, Piece::KNIGHT)) return true;
		if (explore_move(freePosition + Position{+1, +2}, freePosition, Piece::KNIGHT)) return true;
		
		return false;
	}
};

int main() {
	BoardState boardState;
	boardState.init();
	initial = boardState.encode();
	
	const uint32_t SIZE = 0b1111000001u << (ENCODED_SIZE - 10u);
	arr = new uint32_t[SIZE]();
	q.push(initial);
	while (!q.empty()) {
		BoardState boardState{q.front()};
		q.pop();
		if (boardState.explore()) break;
	}
	delete[] arr;
}

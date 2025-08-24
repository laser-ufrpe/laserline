// GENERICS
#define gent_t template<typename T>
#define gents_t template<typename... Ts>
#define gentn_t template<typename T, usize_t N >
#define genkv_t template<typename K, typename V>

// UNSIGNED
using u8_t  = std::uint8_t;
using u16_t = std::uint16_t;
using u32_t = std::uint32_t;
using u64_t = std::uint64_t;

// SIGNED
using i8_t  = std::int8_t;
using i16_t = std::int16_t;
using i32_t = std::int32_t;
using i64_t = std::int64_t;

// FLOAT
using f32_t   = float;
using f64_t   = double;

// SPECIAL
using str_t   = String;
using any_t   = std::any;
using usize_t = std::size_t; 

// GROUP
gent_t using vec = std::vector<T>;
gent_t using set = std::unordered_set<T>;

gentn_t using arr = std::array<T, N>;
gents_t using tup = std::tuple<Ts...>;
genkv_t using dic = std::unordered_map<K, V>;

// LISTS
gent_t using lst = std::list<T>;
gent_t using stk = std::stack<T>;
gent_t using seq = std::queue<T>;
gent_t using deq = std::deque<T>;

// HEAPS
gent_t using maxheap = std::priority_queue<T>;
gent_t using minheap = std::priority_queue<T, std::vector<T>, std::greater<T>>;

#include <experimental/propagate_const>
#include <memory>

class PrivData {
public:
	int a;
};

class C {
public:
	C(){
		m_priv = std::make_unique<PrivData>();
	}

	int getValue() const {
		return m_priv->a;
	}

private:
	std::experimental::propagate_const<std::unique_ptr<PrivData>> m_priv;
};

int main( int argc, char** argv ){
	return 0;
}

int a = 5;

int main(){
	volatile int q = 7;
	volatile int z = q + a;
	return z;
}


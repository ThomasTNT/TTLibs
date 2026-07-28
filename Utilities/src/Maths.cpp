#include <maths.h>

util::Vector::Vector(int size, int arraysize1)
     :length(size), arraysize(arraysize1)
{/*std::cerr << "Vector::Vector " << (void*)this << std::endl; //##########
  if (arraysize < length) arraysize = length;
  array = new std::complex<double>[arraysize];std::cerr << "array: " << (void*)array << std::endl; //##########
  if (!array){ 
    std::cerr << "Vector::Vector() array == NULL!" << std::endl;
    std::cerr << "arraysize: " << arraysize << std::endl;
    std::cerr << "sizeof: " << sizeof(std::complex<double>) << std::endl;
    char c;
    std::cin >> c;
  }*/
  std::complex<double> dummy(0, 0);
  for (int i=0; i<arraysize; i++) array.push_back(dummy);

}

/*util::Vector::Vector(const Vector& other){std::cerr << "Vector::Vector(vector " << (void*)this << " other: " <<(void*)&other << std::endl; //##########
  length = other.length;
  arraysize = other.arraysize;
  //array = new std::complex<double>[arraysize];std::cerr << "array: " << (void*)array << std::endl; //##########
  //for (int i=0; i<length; i++) array[i] = other.array[i];
  for (int i=0; i<arraysize; i++) array.push_back(other.array[i]);
}*/


/*util::Vector::~Vector(){std::cerr << "Vector::~Vector " << (void*)this << std::endl; std::cerr << "array: " << (void*)&array << std::endl; //##########
  //delete[] array;
}*/


/*void util::Vector::add(std::complex<double> value){
  //increase vector if neccessary
  if (size >= arraysize){
    arraysize *= 2;
    std::complex<double>* newarray = new std::complex<double>[arraysize];
    for (int i=0; i<size; i++) newarray[i] = array[i];
    delete[] array;
    array = newarray;
  }
  array[size] = value;
  ++size;
}*/


void util::Vector::set(std::complex<double> value, int index){
  if (index >= 0 && index < length) array[index] = value;
}


std::complex<double> util::Vector::get(int index) const{
  return index >= 0 && index < length ? array[index] : 0;
}


int util::Vector::size() const{
  return length;
}


double util::Vector::absolute() const{
  if (length <= 0) return 0;
  double sum = std::abs(array[0]) * std::abs(array[0]);
  for (int i=0; i<length; i++) sum += std::abs(array[i]) * std::abs(array[i]);
  return sqrt(sum);
}


std::string util::Vector::toString(){
  std::ostringstream os;
  os << "util::Vector(";
  for (int i=0; i<length; i++){
    if (i > 0) os << ", ";
    os << array[i];
  }
  os << ")";
  return os.str();
}


std::complex<double> util::Vector::operator[](int index){
  return get(index);
}


/*util::Vector& util::Vector::operator=(const Vector& other){std::cerr << "Vector::= " << (void*)this << " other: " <<(void*)&other << std::endl; //##########
  if (&other == this) return *this;
  length = other.length;
  arraysize = other.arraysize;
  //delete[] array;
  //array = new std::complex<double>[arraysize];
  //for (int i=0; i<length; i++) array[i] = other.array[i];
  array.clear();
  for (int i=0; i<arraysize; i++) array.push_back(other.array[i]);
  return *this;
}*/

    
util::Vector util::Vector::operator+(const Vector& other){
  Vector temp(length, length);
  for (int i=0; i<length && i<other.length; i++) temp.array[i] = array[i] + other.array[i];
  return temp;
}


util::Vector& util::Vector::operator+=(const Vector& other){
  for (int i=0; i<length && i<other.length; i++) array[i] += other.array[i];
  return *this;
}


util::Vector util::Vector::operator-(const Vector& other){
  Vector temp(length, length);
  for (int i=0; i<length && i<other.length; i++) temp.array[i] = array[i] - other.array[i];
  return temp;
}


util::Vector& util::Vector::operator-=(const Vector& other){
  for (int i=0; i<length && i<other.length; i++) array[i] -= other.array[i];
  return *this;
}

    
util::Vector util::Vector::operator*(std::complex<double> scalar){
  Vector temp(length, length);
  for (int i=0; i<length; i++) temp.array[i] = array[i] * scalar;
  return temp;
}

std::complex<double> util::Vector::operator*(const Vector& other){
  std::complex<double> result = 0;
  for (int i=0; i<length; i++) result += array[i] * other.array[i];
  return result;
}

util::Vector operator*(std::complex<double> scalar, const util::Vector& v){
  util::Vector temp(v.size(), v.size());
  for (int i=0; i<v.size(); i++) temp.set(v.get(i) * scalar, i);
  return temp;
}

util::Vector util::Vector::operator%(const Vector& other){
  Vector temp;
  //later
  return temp;
}


util::Matrix::Matrix(int sx, int sy)
             :sizex(sx), sizey(sy) 
{
  /*if (sizex < 1) sizex = 1;
  if (sizey < 1) sizey = 1;
  matrix = new std::complex<double>*[sizey];   
  for (int y=0; y<sizey; y++) matrix[y] = new std::complex<double>[sizex];*/
  init(std::complex<double>(0,0));
}

util::Matrix::Matrix(const util::Matrix& other){
  sizex = other.sizex;
  sizey = other.sizey;
  /*matrix = new std::complex<double>*[sizey];   
  for (int y=0; y<sizey; y++){
    matrix[y] = new std::complex<double>[sizex];
    for (int x=0; x<sizey; x++){
      (matrix[y])[x] = (other.matrix[y])[x];
    }
  }*/
  for (int y=0; y<sizey; y++){
    std::vector<std::complex<double> > row;
    for (int x=0; x<sizex; x++){
       row.push_back(other.matrix[y][x]);
    }
    matrix.push_back(row);
  }
}

util::Matrix::~Matrix(){
  /*for (int y = 0; y < sizey; y++) delete[] matrix[y];
  delete[] matrix;*/
}

void util::Matrix::set(std::complex<double> value, int x, int y){
  if (x >= 0 && x < sizex && y >= 0 && y < sizey){ 
    (matrix[y])[x] = value;
  }
}

std::complex<double> util::Matrix::get(int x, int y) const{
  return (x >= 0 && x < sizex && y >= 0 && y < sizey) ? (matrix[y])[x] : 0;
}

void util::Matrix::init(std::complex<double> value){
  /*for (int y=0; y<sizey; y++){
    for (int x=0; x<sizex; x++){
      (matrix[y])[x] = value;
    }
  }*/
  for (int y=0; y<sizey; y++){
    std::vector<std::complex<double> > row;
    for (int x=0; x<sizex; x++){
       row.push_back(value);
    }
    matrix.push_back(row);
  }

}

void util::Matrix::setDiagonal(std::complex<double> value){
  for (int y=0; y<sizey; y++){
    for (int x=0; x<sizey; x++){
      (matrix[y])[x] = (x == y) ? value : 0;
    }
  }
}

std::complex<double> util::Matrix::getValue(int x, int y) const{
  return (x >= 0 && x < sizex && y >= 0 && y < sizey) ? (matrix[y])[x] : 0;
}

int util::Matrix::getSizeX() const{
  return sizex;
}

int util::Matrix::getSizeY() const{
  return sizey;
}

util::Vector util::Matrix::getRow(int y) const{
  Vector vec(sizex);
  for (int x = 0; x < sizex; x++) vec.set((matrix[y])[x], x);
  return vec; 
}

util::Vector util::Matrix::getCol(int x) const{
  Vector vec(sizey);
  for (int y = 0; y < sizey; y++) vec.set((matrix[y])[x], y);
  return vec; 
}

//util::Vector& util::Matrix::operator[](int y){
  
//}

util::Matrix& util::Matrix::operator=(const util::Matrix& other){
  if (&other == this) return *this;
  /*for (y = 0; y < sizey; y++) delete[] matrix[y];
  delete[] matrix;*/
  sizex = other.sizex;
  sizey = other.sizey;
  /*matrix = new std::complex<double>*[sizey];   
  for (y=0; y<sizey; y++){
    matrix[y] = new std::complex<double>[sizex];
    for (int x=0; x<sizey; x++){
      (matrix[y])[x] = (other.matrix[y])[x];
    }
  }*/
  matrix.clear();
  for (int y=0; y<sizey; y++){
    std::vector<std::complex<double> > row;
    for (int x=0; x<sizex; x++){
       row.push_back(other.matrix[y][x]);
    }
    matrix.push_back(row);
  }
  return *this;
}
    
util::Matrix util::Matrix::operator+(const util::Matrix& other){
  Matrix temp(sizex, sizey);
  for (int y=0; y<temp.sizey && y<other.sizey; y++){
    for (int x=0; x<temp.sizex && x<other.sizex; x++){
      (temp.matrix[y])[x] = (matrix[y])[x] + (other.matrix[y])[x];
    }
  }
  return temp;
}

util::Matrix util::Matrix::operator+=(const util::Matrix& other){
  for (int y=0; y<sizey && y<other.sizey; y++){
    for (int x=0; x<sizex && x<other.sizex; x++){
      (matrix[y])[x] += (other.matrix[y])[x];
    }
  }
  return *this;
}

util::Matrix util::Matrix::operator-(const util::Matrix& other){
  Matrix temp(sizex, sizey);
  for (int y=0; y<temp.sizey && y<other.sizey; y++){
    for (int x=0; x<temp.sizex && x<other.sizex; x++){
      (temp.matrix[y])[x] = (matrix[y])[x] - (other.matrix[y])[x];
    }
  }
  return temp;
}

util::Matrix util::Matrix::operator-=(const util::Matrix& other){
  for (int y=0; y<sizey && y<other.sizey; y++){
    for (int x=0; x<sizex && x<other.sizex; x++){
      (matrix[y])[x] -= (other.matrix[y])[x];
    }
  }
  return *this;
}
    
util::Matrix util::Matrix::operator*(std::complex<double> scalar){
  Matrix temp(sizex, sizey);
  for (int y=0; y<temp.sizey; y++){
    for (int x=0; x<temp.sizex; x++){
      (temp.matrix[y])[x] = (matrix[y])[x] * scalar;
    }
  }
  return temp;
}

util::Matrix util::Matrix::operator*=(std::complex<double> scalar){
  for (int y=0; y<sizey; y++){
    for (int x=0; x<sizex; x++){
      (matrix[y])[x] *= scalar;
    }
  }
  return *this;
}

util::Vector util::Matrix::operator*(const util::Vector& vec){
  Vector temp(sizey);
  std::complex<double> d;
  for (int y=0; y<sizey; y++){
    d = 0;
    for (int x=0; x<sizex && x<vec.size(); x++){
      d += (matrix[y])[x] * vec.get(x);
    }
    temp.set(d, y);
  }
  return temp;
}

util::Matrix operator*(std::complex<double> scalar, const util::Matrix& m){
  util::Matrix temp(m.getSizeX(), m.getSizeY());
  for (int y=0; y<temp.getSizeY() && y<m.getSizeY(); y++){
    for (int x=0; x<temp.getSizeX() && x<m.getSizeX(); x++){
      temp.set(m.get(x, y) * scalar, x, y);
    }
  }
  return temp;
}

std::string util::Matrix::toString(){
std::ostringstream os;
  os << "util::Matrix(" << std::endl;
  for (int y=0; y<sizey; y++){
    for (int x=0; x<sizex; x++){
      if (x > 0) os << ", ";
      os << (matrix[y])[x];
    }
    os << std::endl;
  }
  os << ")";
  return os.str();
}
#ifndef COMPLEX_H
#define COMPLEX_H

NAMESPACE_BEGIN

class Complex
{
public:
    ~Complex();
    Complex(t_float re = 0.0f, t_float im = 0.0f);
    Complex(const Complex &);


    Complex& operator = (const Complex &);
    //const Complex operator + (const Complex&) const;
    //const Complex operator - (const Complex&) const;
    //const Complex operator * (const Complex&) const;
    //const Complex operator / (const Complex&) const;
    //const Complex operator * (const t_float& f) const;
    //const Complex operator / (const t_float& f) const;

    void operator += (const Complex&);
    void operator -= (const Complex&);
    void operator *= (const Complex&);
    void operator /= (const Complex&);
    //void operator *= (const t_float& f);
    //void operator /= (const t_float& f);

    void conjugue();
    t_float absolute();
	t_bool IsNull();
	t_float real() const;
	t_float imaginary() const;
	void real(t_float r);
	void imaginary(t_float i);

private:
    t_float real_;
    t_float imaginary_;
};


// does that conflict with the operators above???
const Complex operator+(const Complex& lhs, const Complex& rhs);
const Complex operator-(const Complex& lhs, const Complex& rhs);
const Complex operator*(const Complex& lhs, const Complex& rhs);
const Complex operator/(const Complex& lhs, const Complex& rhs);

NAMESPACE_END

#endif // COMPLEX_H

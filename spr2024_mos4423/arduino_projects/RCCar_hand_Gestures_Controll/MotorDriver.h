#ifndef MotorDriver_h
#define MotorDriver_h
class MDOUT{
public:
    void setA(int enb1, int enb2, int phase);
    void setB(int enb1, int enb2, int phase);
    void stby(int stby);

    void stop(bool flag,int sval);
    void forw(bool flag,int sval);
    void revr(bool flag,int sval);
    void brak(bool flag);
private:
    int a_enb1, a_enb2, a_phase;
    int b_enb1, b_enb2, b_phase;
    int m_enb1, m_enb2, m_phase;
    int stpin;
};
#endif


class Config
{
public:
  void read(const char* fileName);
  void save(const char* fileName);
  void mount();
  void reset();
  char* apiEndpoint;
};

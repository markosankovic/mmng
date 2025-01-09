class Master {
public:
  virtual ~Master() {}

  virtual void init(const char *ifname) = 0;

protected:
  Master() {}
};

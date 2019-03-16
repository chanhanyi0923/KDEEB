class RoadSegment {
public:
  size_t oId, dId, density;
  RoadSegment(size_t oId, size_t dId, size_t density):
    oId(oId), dId(dId), density(density)
  {
  }
};

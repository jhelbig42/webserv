#include "Connection.hpp"
#include "Conditions.hpp"
#include <stddef.h>

bool Connection::serve(const size_t Bytes) {
  if (!_req.isFullyParsed()) {
		if (_conditionsFulfilled & _req.getConditions())
			_req.process(_sock, Bytes);
		if (_req.isFullyParsed())
			_res.init(_req);
    return false;
  }
	
  if (_conditionsFulfilled & _res.getConditions())
    return _res.process(_sock, _sockForward, Bytes);
  return false;
}

Conditions Connection::getConditions(void) const {
  if (_req.isFullyParsed())
		return _res.getConditions();
	return _req.getConditions();
}

bool serve(const size_t Bytes) {
  if (_requesting && _conditionsFulfilled & _req.conditions())
    if (_req.process(_sock, Bytes))
      _requesting = false;
    return false;
  }
  _res.init(_req);
  if (_conditionsFulfilled & _res.conditions())
    return _res.process(_sock, _sockForward, Bytes);
  return false;
}

Response::process(const int Socket, const int SocketForward, const size_t Bytes) {
  if (conditions & SockRead)
    //call something
  if (conditions & SockRead)
    //call something
}

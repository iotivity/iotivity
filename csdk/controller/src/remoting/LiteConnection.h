//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef LITECONNECTION_H_
#define LITECONNECTION_H_

// ============================================================================
// Includes
// ============================================================================


// ============================================================================
// Namespace
// ============================================================================
namespace remoting {

  // ==========================================================================
  // Class
  // ==========================================================================

  class LiteConnection {
  public:
    // TODO: socket parameter is faked with an int for now until I know how the connection info will be conveyed
    LiteConnection(int socket);
    virtual ~LiteConnection();

  private:
    int socket_;
  };

}

#endif /* LITECONNECTION_H_ */

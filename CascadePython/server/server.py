from twisted.internet.defer import inlineCallbacks

from autobahn.twisted.util import sleep
from autobahn.twisted.wamp import ApplicationSession
from autobahn.wamp.exception import ApplicationError


class ServerSession(ApplicationSession):

    @inlineCallbacks
    def onJoin(self, details):

        ## REGISTER a procedure for remote calling
        ##
        def send_to_database(*args, **kw):
            print("send_to_database() called with {} and {}".format(args, kw))
            return True 

        reg = yield self.register(send_to_database, 'send_to_database')
        print("procedure send_to_database() registered")



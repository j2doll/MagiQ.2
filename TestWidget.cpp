
#include <QHBoxLayout>

#include "TestWidget.h"
#include "EffectsConstants.h"

TestWidget::TestWidget(QWidget* parent)
	:QWidget(parent)
{
	setMinimumSize(200,200);
	
	Server=new LanServer(this);
	Server->StartListening();
	//ServerThread=new TestWidgetThread(this);

	Client=new LanClient(this);
	LanClient* WhoCares=new LanClient(this);

	BattleFrame=new QFrame;
	QHBoxLayout* BattleFrameLay=new QHBoxLayout(BattleFrame);
	BattleFrameLay->setMargin(0);
	BattleFrameLay->setSpacing(0);
	board=new BattleGround(BattleFrame);
	board->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	BattleFrameLay->addWidget(board);
	StackDisp=new StackDispalyer(BattleFrame);
	BattleFrameLay->addWidget(StackDisp);

	EffectData AddG;
	AddG.SetEffectBody(EffectsConstants::Effects::AddGToManaPool);
	AddG.SetHiddenEffect(true);
	AddG.SetEffectCost(EffectsConstants::EffectCosts::Tap,1);
	AddG.SetVariableValues(1);
	AddG.SetEffectText("Tap: add G to your mana pool");
	AddG.SetEffectType(EffectsConstants::EffectTypes::ActivatedEffect);
	AddG.SetManaEffect(true);
	CardData Foresta;
	Foresta.SetCardName("Forest");
	Foresta.SetHasManaCost(false);
	Foresta.SetAvailableBackgrounds(Constants::CardBacksrounds::LandGreen);
	Foresta.SetCardBackground(Constants::CardBacksrounds::LandGreen);
	Foresta.AddAvailableEditions(Constants::Editions::AVR);
	Foresta.AddCardType(Constants::CardTypes::Basic);
	Foresta.AddCardType(Constants::CardTypes::Land);
	Foresta.AddCardSubType(Constants::CardSubTypes::Forest);
	Foresta.AddEffect(AddG);
	CardData Bear;
	Bear.SetCardName("Grizzly Bears");
	Bear.AddCardCost(Constants::ManaCosts::G,1);
	//Bear.AddCardCost(Constants::ManaCosts::Colorless,1);
	Bear.AddCardColor(Constants::CardColor::Green);
	Bear.SetHasPT(true);
	Bear.SetCardPower(2);
	Bear.SetCardToughness(2);
	Bear.SetAvailableBackgrounds(Constants::CardBacksrounds::Green);
	Bear.SetCardBackground(Constants::CardBacksrounds::Green);
	Bear.AddAvailableEditions(Constants::Editions::ISD);
	Bear.AddCardType(Constants::CardTypes::Creature);
	Bear.AddCardSubType(Constants::CardSubTypes::Beast);
	CardDeck Grimorio;
	for(int i=0;i<30;i++) Grimorio.AddCard(Foresta);
	for(int i=0;i<30;i++) Grimorio.AddCard(Bear);
	Client->SetDeck(Grimorio);
	Client->SetUsername("Tester");
	Client->SetAvatar(QPixmap(":/Board/DefaultAvatar.png"));
	WhoCares->SetDeck(Grimorio);
	WhoCares->SetUsername("Bot");
	WhoCares->SetAvatar(QPixmap(":/Board/DefaultAvatar.png"));

	IPEditor=new QLineEdit(this);
	QRegExpValidator* validatore=new QRegExpValidator(QRegExp("^localhost|(([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5]))$"),IPEditor);
	IPEditor->setValidator(validatore);
	IPEditor->setText("localhost");
	connect(IPEditor,SIGNAL(textEdited(QString)),Client,SLOT(SetHostIP(QString)));
	ConnectButton=new QPushButton(this);
	ConnectButton->setText("Connect");
	connect(ConnectButton,SIGNAL(clicked()),Client,SLOT(ConnectToHost()));
	JoinButton=new QPushButton(this);
	JoinButton->setText("Join");
	connect(JoinButton,SIGNAL(clicked()),Client,SLOT(SendJoinRequest()));
	DisconnectButton=new QPushButton(this);
	DisconnectButton->setText("Leave");
	connect(DisconnectButton,SIGNAL(clicked()),Client,SLOT(disconnectFromHost()));
	ReadyButton=new QPushButton(this);
	ReadyButton->setText("Ready");
	connect(ReadyButton,SIGNAL(clicked()),Client,SLOT(SendReady()));

	chat=new ChatWidget(this);

	QGridLayout* MainLay=new QGridLayout(this);
	MainLay->addWidget(IPEditor,0,0);
	MainLay->addWidget(ConnectButton,1,0);
	MainLay->addWidget(JoinButton,2,0);
	MainLay->addWidget(ReadyButton,3,0);
	MainLay->addWidget(DisconnectButton,4,0);
	MainLay->addWidget(chat,0,1,5,1);

	//Chat Connections
	connect(Client,SIGNAL(Disconnected()),chat,SLOT(Disconnected()));
	connect(Client,SIGNAL(UserJoined(QString)),chat,SLOT(AnotherJoin(QString)));
	connect(Client,SIGNAL(UserLeft(QString)),chat,SLOT(AnotherLeave(QString)));
	connect(Client,SIGNAL(MyNameAndColor(QString,QColor)),chat,SLOT(Connected()));
	connect(Client,SIGNAL(MyNameAndColor(QString,QColor)),chat,SLOT(SetNameColor(QString,QColor)));
	connect(Client,SIGNAL(error(QAbstractSocket::SocketError)),chat,SLOT(DisplayNetworkErrors(QAbstractSocket::SocketError)));
	connect(chat,SIGNAL(OutgoingMessage(QString)),Client,SLOT(SendChatMessage(QString)));
	connect(Client,SIGNAL(ChatMessageRecieved(QString)),chat,SLOT(IncomingMesage(QString)));

	//BattleGroundConnection
	connect(Client,SIGNAL(PlayOrder(QList<int>)),board,SLOT(SetPlayersOrder(QList<int>)));
	connect(Client,SIGNAL(PlayersNameAvatar(QMap<int,QString>,QMap<int,QPixmap>)),board,SLOT(SetPlayersNameAvatar(QMap<int,QString>,QMap<int,QPixmap>)));
	connect(Client,SIGNAL(AllCards(QList<CardData>)),board,SLOT(SetAllCards(QList<CardData>)));
	connect(Client,SIGNAL(IsMyTurn(bool)),board,SLOT(SetYourTurn(bool)));
	connect(Client,SIGNAL(MyHand(QList<int>)),board,SLOT(SetMyHand(QList<int>)));
	connect(Client,SIGNAL(MyHand(QList<int>)),board,SLOT(AskMulligan()));
	connect(Client,SIGNAL(OtherHand(int,int)),board,SLOT(SetOtherHand(int,int)));
	connect(Client,SIGNAL(PlayerLibrary(int,int)),board,SLOT(SetPlayerLibrary(int,int)));
	connect(Client,SIGNAL(WaitingFor(QString)),board,SLOT(DispalyWaitingFor(QString)));
	connect(Client,SIGNAL(StopWaitingFor()),board,SLOT(HideWaitingFor()));
	connect(board,SIGNAL(Mulligan()),Client,SLOT(SendMulligan()));
	connect(board,SIGNAL(KeepHand()),Client,SLOT(SendHandAccepted()));
	connect(Client,SIGNAL(CurrentPhaseChanged(int)),board,SLOT(SetCurrentPhase(int)));
	connect(Client,SIGNAL(CardsToUntap(QList<int>)),board,SLOT(UntapCards(QList<int>)));
	connect(Client,SIGNAL(CardDrawn(int)),board,SLOT(DrawCard(int)));
	connect(Client,SIGNAL(OtherDrawn(int)),board,SLOT(OtherDraw(int)));
	connect(board,SIGNAL(TimerFinished()),Client,SLOT(SendFinishedTimer()));
	connect(board,SIGNAL(TimerStopped()),Client,SLOT(SendStoppedTimer()));
	connect(board,SIGNAL(TimerResumed()),Client,SLOT(SendResumeTimer()));
	connect(Client,SIGNAL(StopTimer()),board,SLOT(StopTimer()));
	connect(Client,SIGNAL(StopTurnTimer()),board,SLOT(StopTurnTimer()));
	connect(Client,SIGNAL(ResumeTurnTimer()),board,SLOT(ResumeTurnTimer()));
	connect(Client,SIGNAL(ResumeStackTimer()),board,SLOT(ResumeStackTimer()));
	connect(Client,SIGNAL(EffectAddedToStack(EffectData,quint32)),board,SLOT(ResumeStackTimer()));
	connect(Client,SIGNAL(PlayableCards(QList<int>)),board,SLOT(SetPlayableCards(QList<int>)));
	connect(board,SIGNAL(WantPlayCard(int,QList<int>)),Client,SLOT(SendWantToPlayCard(int,QList<int>)));
	connect(Client,SIGNAL(PlayedCard(int,int)),board,SLOT(PlayedCard(int,int)));
	connect(Client,SIGNAL(RemoveFromHand(int,int)),board,SLOT(RemoveCardHand(int,int)));
	connect(Client,SIGNAL(PermanentResolved(int,int)),board,SLOT(ResolveCard(int,int)));
	connect(Client,SIGNAL(CardsToTap(QList<int>)),board,SLOT(TapCards(QList<int>)));
	connect(board,SIGNAL(ContinueToNextPhase()),Client,SLOT(SendContinueToNextPhase()));
	connect(Client,SIGNAL(AttackAbleCards(QList<int>)),board,SLOT(SetAttackAbleCards(QList<int>)));
	connect(board,SIGNAL(SendAttackingCards(const QHash<int,int>&)),Client,SLOT(SendAttackingCards(const QHash<int,int>&)));
	connect(Client,SIGNAL(AttackingCards(QHash<int,int>)),board,SLOT(SetAttackingCards(QHash<int,int>)));

	//StackDisplayerConnections
	connect(Client,SIGNAL(EffectAddedToStack(EffectData,quint32)),StackDisp,SLOT(AddEffect(EffectData)));
	//connect(Client,SIGNAL(PlayedCard(CardData,int)),StackDisp,SLOT(AddCard(CardData)));
	connect(Client,SIGNAL(EffectResolved()),StackDisp,SLOT(Resolve()));

	//Test Connections
	connect(Client,SIGNAL(GameHasStarted()),BattleFrame,SLOT(show()));
	connect(board,SIGNAL(KeepHand()),WhoCares,SLOT(SendHandAccepted()));
	connect(board,SIGNAL(TimerFinished()),WhoCares,SLOT(SendFinishedTimer()));
	QPushButton* PauseTimerButton=new QPushButton(this);
	PauseTimerButton->setObjectName("PauseTimerButton");
	PauseTimerButton->setText("Pause Timer");
	connect(PauseTimerButton,SIGNAL(clicked()),WhoCares,SLOT(SendStoppedTimer()));
	QPushButton* ResumeTimerButton=new QPushButton(this);
	ResumeTimerButton->setObjectName("PauseTimerButton");
	ResumeTimerButton->setText("Resume Timer");
	connect(ResumeTimerButton,SIGNAL(clicked()),WhoCares,SLOT(SendResumeTimer()));
	MainLay->addWidget(PauseTimerButton,0,2);
	MainLay->addWidget(ResumeTimerButton,1,2);

	WhoCares->ConnectToHost();
	WhoCares->SendJoinRequest();
	WhoCares->SendReady();
}

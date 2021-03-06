#include "CardData.h"
#include <QMessageBox>
CardData::CardData()
	:CardEdition(0)
	,CardPower(0)
	,CardToughness(0)
	,CardImage(0)
	,HasPT(false)
	,Certified(false)
	,CardName("")
	,CardFlavorText("")
	,HasFlipped(Constants::CardFlipMode::NoFlip)
	,HasManaCost(true)
	,CardBackground(Constants::CardBacksrounds::Colorless)
	,CardRarity(Constants::CardRarities::Common)
	,SortingMethod(Random)
	,CardID(0)
	,Owner(NULL)
	,Controller(NULL)
	,Tapped(false)
	,IsNull(false)
	,Activable(false)
	,SummoningSickness(false)
	//,CanAttack(false)
	,CanAttack(true) //DEBUG
	,Attacking(false)
{}
CardData::CardData(const CardData& a)
	:CardEdition(a.CardEdition)
	,CardID(a.CardID)
	,CardPower(a.CardPower)
	,CardToughness(a.CardToughness)
	,CardImage(a.CardImage)
	,HasPT(a.HasPT)
	,Certified(a.Certified)
	,CardName(a.CardName)
	,CardFlavorText(a.CardFlavorText)
	,HasFlipped(a.HasFlipped)
	,HasManaCost(a.HasManaCost)
	,CardBackground(a.CardBackground)
	,CardRarity(a.CardRarity)
	,SortingMethod(a.SortingMethod)
	,CardColor(a.CardColor)
	,CardCost(a.CardCost)
	,CardType(a.CardType)
	,CardSubType(a.CardSubType)
	,AvailableEditions(a.AvailableEditions)
	,AvailableImages(a.AvailableImages)
	,AvailableBackgrounds(a.AvailableBackgrounds)
	,Effects(a.Effects)
	,Owner(a.Owner)
	,Controller(a.Controller)
	,Tapped(a.Tapped)
	,IsNull(a.IsNull)
	,Activable(a.Activable)
	,SummoningSickness(a.SummoningSickness)
	,CanAttack(a.CanAttack)
	,Attacking(a.Attacking)
{
	if(HasFlipped==Constants::CardFlipMode::HasFlip){
		SetFlippedCard(new CardData(*(a.FlippedCard)));
		FlippedCard->SetFlippedCard(this);
	}
}
const CardData& CardData::operator=(const CardData& a){
	CardEdition=(a.CardEdition);
	CardID=a.CardID;
	IsNull=a.IsNull;
	Activable=a.Activable;
	Attacking=a.Attacking;
	SummoningSickness=a.SummoningSickness;
	CanAttack=a.CanAttack;
	Owner=(a.Owner);
	Controller=(a.Controller);
	Tapped=a.Tapped;
	CardPower=(a.CardPower);
	CardToughness=(a.CardToughness);
	CardImage=(a.CardImage);
	HasPT=(a.HasPT);
	Certified=(a.Certified);
	CardName=(a.CardName);
	CardFlavorText=(a.CardFlavorText);
	HasFlipped=(a.HasFlipped);
	HasManaCost=(a.HasManaCost);
	CardBackground=(a.CardBackground);
	CardRarity=(a.CardRarity);
	SortingMethod=(a.SortingMethod);
	CardColor.clear(); CardColor=(a.CardColor);
	CardCost.clear(); CardCost=(a.CardCost);
	CardType.clear(); CardType=(a.CardType);
	CardSubType.clear(); CardSubType=(a.CardSubType);
	AvailableEditions.clear(); AvailableEditions=(a.AvailableEditions);
	AvailableImages.clear(); AvailableImages=(a.AvailableImages);
	AvailableBackgrounds.clear(); AvailableBackgrounds=(a.AvailableBackgrounds);
	Effects.clear(); Effects=(a.Effects);
	if(HasFlipped==Constants::CardFlipMode::HasFlip){
		SetFlippedCard(new CardData(*(a.FlippedCard)));
		FlippedCard->SetFlippedCard(this);
	}
	return *this;
}
bool CardData::operator<(const CardData& a) const{
	if (a.GetIsNull()) return !IsNull;
	switch(SortingMethod){
	case ByName:
		return CardName<a.CardName;
	case ByManaCost:
		return GetConvertedManaCost()<a.GetConvertedManaCost();
	case ByType:
		for (int i=0;i<Constants::CardTypes::END;i++){
			if (CardType.contains(i)) return true;
			if (a.CardType.contains(i)) return false;
		}
		return false;
	case ByColor:
		for (int i=0;i<=Constants::CardColor::Green;i++){
			if (CardColor.contains(i)) return true;
			if (a.CardColor.contains(i)) return false;
		}
		return false;
	case Random:
	default:
		return qrand()%2==0;
	}
}
int CardData::GetConvertedManaCost() const{
	int Result=0;
	for (int i=0;i<Constants::ManaCosts::END;i++){
		if (i>=Constants::ManaCosts::C2W && i<=Constants::ManaCosts::C2G)
			Result+=2*CardCost[i];
		else Result+=CardCost[i];
	}
	return Result;
}

void CardData::SetCardCost(){
	if (!CardCost.isEmpty())
		CardCost.clear();
	for (int i=Constants::ManaCosts::Colorless;i<Constants::ManaCosts::END;i++)
		CardCost.insert(i,0);
}
void CardData::AddCardCost(int key, int cost){
	if (key<Constants::ManaCosts::Colorless || key>=Constants::ManaCosts::END){
		return;
	}
	CardCost[key]+=cost;
}
void CardData::AddAvailableEditions(const int& a){
	if (a>=0 && a<Constants::Editions::END){
		AvailableEditions.append(a);
		if (AvailableEditions.contains(Constants::Editions::NONE))
			AvailableEditions.erase(AvailableEditions.begin()+AvailableEditions.indexOf(Constants::Editions::NONE));
		if(AvailableEditions.size()==1)
			CardEdition=AvailableEditions.at(0);
	}
}
void CardData::SetAvailableImages(const QPixmap& a){
	AvailableImages.clear();
	AvailableImages.append(a);
}
void CardData::SetAvailableImages(){AvailableImages.clear();}
void CardData::AddAvailableImages(const QPixmap& a){AvailableImages.append(a);}
void CardData::SetAvailableImages(const QList<QPixmap>& a){
	AvailableImages.clear();
	AvailableImages=a;
}
QDataStream &operator<<(QDataStream &out, const CardData &card)
{
	out << qint32(Constants::CardVersion)
		<< quint32(card.GetCardID())
		<< card.GetCardName()
		<< card.GetCardColor()
		<< card.GetCardCost()
		<< card.GetCardType()
		<< card.GetCardSubType()
		<< card.GetAvailableEditions()
		<< card.GetAvailableImages()
		<< card.GetImagesTitles()
		<< card.GetAvailableBackgrounds()
		<< qint32(card.GetCardEdition())
		<< qint32(card.GetCardBackground())
		<< card.GetCardFlavorText()
		<< card.GetHasPT()
		<< qint32(card.GetCardPower())
		<< qint32(card.GetCardToughness())
		<< qint32(card.GetCardRarity())
		<< qint32(card.GetCardImage())
		<< card.GetCertified()
		<< qint32(card.GetCertifiedCardID())
		<< card.GetHasManaCost()
		<< qint32(card.GetEffects().size())
		<< qint32(card.GetHasFlipped())
		;
	foreach(EffectData eff,card.GetEffects())
		out << eff;
	if (card.GetHasFlipped()==Constants::CardFlipMode::HasFlip)
		out << *(card.GetFlippedCard());
	return out;
}
QDataStream &operator>>(QDataStream &input, CardData &card){
	qint32 Numbers;
	quint32 UnsignedNumbers;
	qint32 NumberOfEffects;
	EffectData effect;
	QString Strings;
	QStringList StrList;
	QList<int> IntLists;
	QMap<int,int> IntMap;
	QList<QPixmap> ImageLists;
	bool Booleans;
	input >> Numbers;
	if (Numbers!=Constants::CardVersion){
		QMessageBox::critical(0,QObject::tr("Wrong File Type"),QObject::tr("Error: Unable to Read the File\nMake sure it's a valid MagiQ Card File"));
		return input;
	}
	input >> UnsignedNumbers;
	card.SetCardID(UnsignedNumbers);
	input >> Strings;
	card.SetCardName(Strings);
	input >> IntLists;
	card.SetCardColor(IntLists);
	input >> IntMap;
	card.SetCardCost(IntMap);
	IntLists.clear();
	IntLists.clear();
	input >> IntLists;
	card.SetCardType(IntLists);
	IntLists.clear();
	input >> IntLists;
	card.SetCardSubType(IntLists);
	IntLists.clear();
	input >> IntLists;
	card.SetAvailableEditions(IntLists);
	input >> ImageLists;
	card.SetAvailableImages(ImageLists);
	input >> StrList;
	card.SetImagesTitles(StrList);
	IntLists.clear();
	input >> IntLists;
	card.SetAvailableBackgrounds(IntLists);
	input >> Numbers;
	card.SetCardEdition(Numbers);
	input >> Numbers;
	card.SetCardBackground(Numbers);
	input >> Strings;
	card.SetCardFlavorText(Strings);
	input >> Booleans;
	card.SetHasPT(Booleans);
	input >> Numbers;
	card.SetCardPower(Numbers);
	input >> Numbers;
	card.SetCardToughness(Numbers);
	input >> Numbers;
	card.SetCardrarity(Numbers);
	input >> Numbers;
	card.SetCardImage(Numbers);
	input >> Booleans;
	card.SetCertified(Booleans);
	input >> Numbers;
	card.SetCertifiedCardID(Numbers);
	input >> Booleans;
	card.SetHasManaCost(Booleans);
	input >> NumberOfEffects;
	input >> Numbers;
	card.SetHasFlipped(Numbers);
	card.SetEffects();
	for (int i=0;i<NumberOfEffects;i++){
		input >> effect;
		card.AddEffect(effect);
	}
	if (Numbers==Constants::CardFlipMode::HasFlip){
		CardData* TempCard=new CardData;
		input >> *TempCard;
		TempCard->SetFlippedCard(&card);
		card.SetFlippedCard(TempCard);
	}
	return input;
}
CardData CardData::ToNoImage() const {
	CardData Result(*this);
	Result.SetAvailableImages();
	Result.SetImagesTitles();
	return Result;
}
void CardData::SetEffectID(int index,int ID){
	if (index<0 || index>Effects.size()) return;
	Effects[index].SetEffectID(ID);
}
void CardData::SetEffects(const QList<EffectData>& a){
	Effects.clear(); 
	Effects=a;
	for (QList<EffectData>::iterator i=Effects.begin();i!=Effects.end();i++)
		i->SetCardAttached(this);
}